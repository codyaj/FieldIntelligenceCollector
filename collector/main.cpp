#include <cstdlib>
#include <pcap.h>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <chrono>
#include "include/AdapterManager.hpp"
#include "include/Database.hpp"


int main() {
    if (geteuid() != 0) {
        std::cout << "This program must be run as sudo!" << std::endl;
        return 1;
    }

    DatabaseManager databaseManager("../data/data.db"); // Relative path from build file

    return 0;

    AdapterManager adapterManager("wlp0s20f0u5u3");

    if(!adapterManager.isMonitorMode()) {
        std::cout << "Enabling monitor mode" << std::endl;
        if(!adapterManager.setMonitorMode()) {
            std::cout << "Failed to set to monitor mode" << std::endl;
            return 1;
        }
    }

    std::cout << "Changing to channel " << adapterManager.getSupportedChannels()[4] << std::endl;
    adapterManager.setChannel(adapterManager.getSupportedChannels()[4]);

    return 0;


    /*
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(iface.c_str(), 65535, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "Failed to open device: " << errbuf << std::endl;
        return 1;
    }

    std::vector<int> channels = {1,2,3,4,5,6,7,8,9,10,11};
    int dwell_ms = 200;

    while (true) {
        for (int ch : channels) {

            // Capture packets for dwell_ms
            auto start = std::chrono::steady_clock::now();
            while (true) {
                pcap_dispatch(handle, 0, [](u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
                    std::cout << "Packet length: " << header->len << std::endl;
                }, nullptr);

                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() >= dwell_ms)
                    break;
            }
        }
    }

    pcap_close(handle);
    return 0;*/
}
