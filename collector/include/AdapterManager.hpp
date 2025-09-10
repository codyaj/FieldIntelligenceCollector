#pragma once

#include <string>
#include <vector>
#include <regex>
#include <iostream>

class AdapterManager {
private:
    std::string iface;
    std::vector<int> supportedChannels;
    std::vector<int> activeChannels;

    bool detectSupportedChannels();
public:
    explicit AdapterManager(const std::string &iface) : iface(iface) {
        if (!detectSupportedChannels()) {
            std::cout << "Failed to determine supported channels" << std::endl;
        }

        activeChannels = supportedChannels;
    }

    // Monitor mode functions
    bool isMonitorMode() const;
    bool setMonitorMode();

    // Channel control
    bool setChannel(int channel);
    std::vector<int> getSupportedChannels() const { return supportedChannels; }
    std::vector<int> getActiveChannels() const { return activeChannels; }

    // Active channel management
    void setActiveChannels(const std::vector<int>& channels);
};