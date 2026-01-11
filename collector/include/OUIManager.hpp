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

    uint32_t parse_OUI(const std::string& ouiStr);

    void load_OUI_map(const std::string& path);
    
public:
    explicit OUIManager(const std::string& path) { load_OUI_map(path); }

    std::string lookup_MAC(const std::string& macStr);
};