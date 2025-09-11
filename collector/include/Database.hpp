#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>

struct Packet {
    std::string source_mac;
    std::string dest_mac;
    std::string bssid;
    std::string protocol;
    int payload_size;
    std::string timestamp; // ISO 8601 string
    double latitude;
    double longitude;
    int channel;
    std::string source_oui_vendor;
    std::string dest_oui_vendor;
    std::string metadata; // JSON
};

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbPath;

    bool open();
    void close();
    bool initTables();
public:
    explicit DatabaseManager(const std::string& path) : dbPath(path) { 
        if (open()) {
            initTables();
        }
    }
    ~DatabaseManager() { close(); }

    bool insertPackets(const std::vector<Packet>& packets);

    std::vector<int> getActiveChannels();

    bool logSupportedChannels(const std::vector<int>& channels);
};