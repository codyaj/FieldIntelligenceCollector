#include "./Database.hpp"

bool DatabaseManager::open() {
    int errorCode = sqlite3_open(dbPath.c_str(), &db);
    if (errorCode != SQLITE_OK) {
        std::cerr << "Failed to open or create sqlite db. Error code: " << errorCode << std::endl;
        return false;
    }
    return true;
}

void DatabaseManager::close() {
    if (db) sqlite3_close(db);
    db = nullptr;
}

bool DatabaseManager::initTables() {
    if (!db) {
        std::cerr << "Database not open. Call open() first." << std::endl;
        return false;
    }

    const char* packetsTable = R"SQL(
    CREATE TABLE IF NOT EXISTS packets_table(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        source_mac TEXT NOT NULL,
        dest_mac TEXT NOT NULL,
        bssid TEXT NO NULL,
        protocol TEXT,
        payload_size INTEGER,
        timestamp TEXT NOT NULL,
        latitude REAL NOT NULL,
        longitude REAL NOT NULL,
        channel INT NOT NULL,
        source_oui_vendor TEXT,
        dest_oui_vendor TEXT,
        metadata TEXT
    );)SQL";

    const char* sourceIndex = "CREATE INDEX IF NOT EXISTS idx_packets_source_mac ON packets_table(source_mac);";
    const char* destIndex = "CREATE INDEX IF NOT EXISTS idx_packets_dest_mac ON packets_table(dest_mac);";
    const char* timeIndex = "CREATE INDEX IF NOT EXISTS idx_packets_timestamp ON packets_table(timestamp);";

    const char* supportedChannelsTable = R"SQL(
    CREATE TABLE IF NOT EXISTS supported_channels (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        channel INTEGER NOT NULL
    );)SQL";

    const char* scanConfigTable = R"SQL(
    CREATE TABLE IF NOT EXISTS scan_config (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        channel INTEGER NOT NULL
    );)SQL";

    const char* schema[] = {packetsTable, sourceIndex, destIndex, timeIndex, supportedChannelsTable, scanConfigTable};

    for (const char* sql : schema) {
        char *err = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
        if (rc != SQLITE_OK) {
            std::cerr << "SQLite error (" << rc << "): " << (err ? err : "unknown") << std::endl;
            if (err) sqlite3_free(err);
            return false;
        }
        if (err) sqlite3_free(err); // Free in case returned warning
    }

    return true;
}

bool DatabaseManager::insertPackets(const std::vector<Packet>& packets) {
    if (!db) {
        std::cerr << "Database not open. Call open() first." << std::endl;
        return false;
    }

    // Insert new packets
    const char* packetsSQL = R"SQL(
        INSERT INTO packets_table (source_mac, dest_mac, bssid, protocol, payload_size, timestamp, latitude, longitude, channel, source_oui_vendor, dest_oui_vendor, metadata)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, packetsSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Begin transaction
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
        std::cerr << "Failed to begin transaction: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    for (const Packet& packet : packets) {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        sqlite3_bind_text(stmt, 1, packet.source_mac.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, packet.dest_mac.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, packet.bssid.c_str(), -1, SQLITE_TRANSIENT);
        if (packet.protocol.empty()) {
            sqlite3_bind_null(stmt, 4);
        } else { 
            sqlite3_bind_text(stmt, 4, packet.protocol.c_str(), -1, SQLITE_TRANSIENT);
        }
        if (packet.payload_size < 0) {
            sqlite3_bind_null(stmt, 5);
        } else {
            sqlite3_bind_int(stmt, 5, packet.payload_size);
        }
        sqlite3_bind_text(stmt, 6, packet.timestamp.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 7, packet.latitude);
        sqlite3_bind_double(stmt, 8, packet.longitude);
        sqlite3_bind_int(stmt, 9, packet.channel);
        if (packet.source_oui_vendor.empty()) {
            sqlite3_bind_null(stmt, 10);
        } else {
            sqlite3_bind_text(stmt, 10, packet.source_oui_vendor.c_str(), -1, SQLITE_TRANSIENT);
        }
        if (packet.dest_oui_vendor.empty()) {
            sqlite3_bind_null(stmt, 11);
        } else {
            sqlite3_bind_text(stmt, 11, packet.dest_oui_vendor.c_str(), -1, SQLITE_TRANSIENT);
        }
        if (packet.metadata.empty()) {
            sqlite3_bind_null(stmt, 12);
        } else {
            sqlite3_bind_text(stmt, 12, packet.metadata.c_str(), -1, SQLITE_TRANSIENT);
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Insert failed: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            sqlite3_finalize(stmt);
            return false;
        }
    }

    // Commit transaction
    rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to commit transaction: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    return true;
}

std::vector<int> DatabaseManager::getActiveChannels() {
    std::vector<int> channels;
    if (!db) {
        std::cerr << "Database not open. Call open() first." << std::endl;
        return channels;
    }

    const char* sql = "SELECT channel FROM supported_channels;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return channels;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int chan = sqlite3_column_int(stmt, 0);
        channels.push_back(chan);
    }

    sqlite3_finalize(stmt);
    return channels;
}

bool DatabaseManager::logSupportedChannels(const std::vector<int>& channels) {
    if (!db) {
        std::cerr << "Database not open. Call open() first." << std::endl;
        return false;
    }

    // Clear table
    const char* clearSQL = "DELETE FROM supported_channels";
    const char* resetIncrement = "DELETE FROM sqlite_sequence WHERE name='supported_channels'";
    const char* vaccum = "VACUUM";

    const char* schema[] = {clearSQL, resetIncrement, vaccum};

    for (const char* sql : schema) {
        char *err = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
        if (rc != SQLITE_OK) {
            std::cerr << "SQLite error (" << rc << "): " << (err ? err : "unknown") << std::endl;
            if (err) sqlite3_free(err);
            return false;
        }
        if (err) sqlite3_free(err); // Free in case returned warning
    }


    // Insert new supported channels
    const char* channelsSQL = R"SQL(
        INSERT INTO supported_channels (channel)
        VALUES (?);
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, channelsSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Begin transaction
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
        std::cerr << "Failed to begin transaction: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    for (const int& channel : channels) {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        sqlite3_bind_int(stmt, 1, channel);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Insert failed: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            sqlite3_finalize(stmt);
            return false;
        }
    }

    // Commit transaction
    rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to commit transaction: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    return true;
}
