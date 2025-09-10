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
        protocol TEXT,
        payload_size INTEGER,
        timestamp TEXT NOT NULL,
        latitude REAL,
        longitude REAL,
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

    

    return true;
}

std::vector<int> DatabaseManager::getActiveChannels() {
    std::vector<int> channels;
    if (!db) return channels;



    return channels;
}

bool DatabaseManager::logSupportedChannels(std::vector<int>& channels) {
    if (!db) {
        std::cerr << "Database not open. Call open() first." << std::endl;
        return false;
    }

    

    return true;
}
