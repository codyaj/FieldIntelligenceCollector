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
    double gnss_fix_time_utc;
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
    bool init_tables();
public:
    explicit DatabaseManager(const std::string& path) : dbPath(path) { 
        if (open()) {
            init_tables();
        }
    }
    ~DatabaseManager() { close(); }

    bool insert_packets(const std::vector<Packet>& packets);

    std::vector<int> get_active_channels();

    bool log_supported_channels(const std::vector<int>& channels);
};