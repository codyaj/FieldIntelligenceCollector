#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include "Database.hpp" // For packet
#include "OUIManager.hpp"

struct radiotap_header {
    uint8_t  it_version;     // always 0
    uint8_t  it_pad;
    uint16_t it_len;         // total length of radiotap header
    uint32_t it_present[];   // bitmap, variable
};

class PacketScanner {
private:
    std::vector<Packet> batchBuffer;
    size_t batchSizeThreshold;
    std::chrono::milliseconds batchTimeThreshold;
    std::chrono::steady_clock::time_point lastBatchTime;

public:
    explicit PacketScanner(size_t batchSize = 200, int batchTimeMs = 2000) : batchSizeThreshold(batchSize), 
                                                                             batchTimeThreshold(std::chrono::milliseconds(batchTimeMs)),
                                                                             lastBatchTime(std::chrono::steady_clock::now()) {}

    Packet processPacket(const uint8_t* packetData, size_t length, const std::string& timestamp, int currChannel);

    void addToBatch(Packet packet) { batchBuffer.push_back(packet); }

    bool isBatchReady();

    std::vector<Packet> getBatch();
};