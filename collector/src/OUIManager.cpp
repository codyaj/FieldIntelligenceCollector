#include "./OUIManager.hpp"


uint32_t OUIManager::parse_OUI(const std::string& ouiStr) {
    uint32_t oui = 0;
    std::istringstream ss(ouiStr);
    std::string byteStr;
    for (int i = 0; i < 3; ++i) {  // OUI is 3 bytes
        if (!std::getline(ss, byteStr, ':')) break;
        oui = (oui << 8) | std::stoi(byteStr, nullptr, 16);
    }
    return oui;
}

void OUIManager::load_OUI_map(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OUI CSV file!" << std::endl;
        return;
    }

    std::string line;
    // skip header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string registry, assignment, orgName, orgAddr;

        if (!std::getline(ss, registry, ',')) continue;
        if (!std::getline(ss, assignment, ',')) continue;
        if (!std::getline(ss, orgName, ',')) continue;
        // orgAddr is ignored

        uint32_t oui = parse_OUI(assignment);
        ouiMapAll[oui] = orgName;
    }

    file.close();
}

std::string OUIManager::lookup_MAC(const std::string& macStr) {
    uint32_t oui = parse_OUI(macStr.substr(0, 8));

    // Search cache
    auto itCache = ouiMapCache.find(oui);
    if (itCache != ouiMapCache.end()) return itCache->second;

    // Search main
    auto itMain = ouiMapAll.find(oui);
    if (itMain != ouiMapAll.end()) {
        ouiMapCache[oui] = itMain->second;
        return itMain->second;
    }

    return "Unknown";
}
