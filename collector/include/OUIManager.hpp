#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <cinttypes>
#include <iostream>

class OUIManager {
private:
    std::unordered_map<uint32_t, std::string> ouiMapAll;
    std::unordered_map<uint32_t, std::string> ouiMapCache;

    uint32_t parseOUI(const std::string& ouiStr);

    void loadOUIMap(const std::string& path);
public:
    explicit OUIManager(const std::string& path) { loadOUIMap(path); }

    std::string lookupMAC(const std::string& macStr);
};