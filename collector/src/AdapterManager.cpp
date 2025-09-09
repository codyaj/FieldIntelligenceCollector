#include "./AdapterManager.hpp"

bool AdapterManager::isMonitorMode() const {
    std::string cmd = "iw dev " + iface + " info | grep 'type'";
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) return false;
    
        char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    return result.find("monitor") != std::string::npos;
}

bool AdapterManager::setMonitorMode() {
    if (system(("ip link set " + iface + " down").c_str()) != 0)
        return false;
    if (system(("iw " + iface + " set monitor control").c_str()) != 0)
        return false;
    if (system(("ip link set " + iface + " up").c_str()) != 0)
        return false;
    return true;
}

bool AdapterManager::setChannel(int channel) {
    std::string cmd = "iw dev " + iface + " set channel " + std::to_string(channel);
    return system(cmd.c_str()) == 0;
}

bool AdapterManager::detectSupportedChannels() {
    AdapterManager::supportedChannels.clear();

    // Find PHY name
    std::string phyCmd = "iw dev " + iface + " info | grep wiphy | awk '{print $2}'";
    FILE* pipe = popen(phyCmd.c_str(), "r");
    if (!pipe) return false;
    char buffer[64];
    std::string phy;
    if (fgets(buffer, sizeof(buffer), pipe)) {
        phy = "phy" + std::string(buffer);
        // trim newline
        phy.erase(phy.find_last_not_of(" \n\r\t") + 1);
    }
    pclose(pipe);
    if (phy.empty()) return false;

    // Get phy info
    std::string cmd = "iw " + phy + " info";
    pipe = popen(cmd.c_str(), "r");
    if (!pipe) return false;

    bool inFreqSection = false;
    std::regex chanRegex("\\[(\\d+)\\]");
    char line[256];

    while (fgets(line, sizeof(line), pipe)) {
        std::string str(line);

        if (str.find("Frequencies:") != std::string::npos) {
            inFreqSection = true;
            continue;
        }

        if (inFreqSection) {
            if (str.find('*') == std::string::npos) {
                inFreqSection = false;
                continue;
            }
            std::smatch match;
            if (std::regex_search(str, match, chanRegex)) {
                supportedChannels.push_back(std::stoi(match[1]));
            }
        }
    }
    pclose(pipe);

    return true;
}

void AdapterManager::setActiveChannels(const std::vector<int>& channels) {
    for (int channel : channels) {
        if (std::find(AdapterManager::supportedChannels.begin(), AdapterManager::supportedChannels.end(), channel) == AdapterManager::supportedChannels.end()) {
            std::cout << "Failed to update active channels. " << channel << " is not a supported channel." << std::endl;
            return;
        }
    }
    activeChannels = channels;
}