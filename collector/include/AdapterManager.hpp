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
    int currentChannel;

    bool detect_supported_channels();

public:
    explicit AdapterManager(const std::string &iface) : iface(iface) {
        if (!detect_supported_channels()) {
            std::cout << "Failed to determine supported channels" << std::endl;
        }

        activeChannels = supportedChannels;
    }

    // Monitor mode functions
    bool is_monitor_mode() const;
    bool set_monitor_mode();

    // Channel control
    bool set_channel(int channel);
    int get_current_channel() { return currentChannel; };
    std::vector<int> get_supported_channels() const { return supportedChannels; }
    std::vector<int> get_active_channels() const { return activeChannels; }

    // Active channel management
    void set_active_channels(const std::vector<int>& channels);
};