#include <cstdlib>
#include <pcap.h>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <chrono>

bool isMonitorMode(const std::string &iface) {
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

void setMonitorMode(const std::string &iface) {
    if (system(("ip link set " + iface + " down").c_str()) != 0)
        exit(1);
    if (system(("iw " + iface + " set monitor control").c_str()) != 0)
        exit(1);
    if (system(("ip link set " + iface + " up").c_str()) != 0)
        exit(1);
}

void setChannel(const std::string &iface, int channel) {
    std::string cmd = "iw dev " + iface + " set channel " + std::to_string(channel);
    if (system(cmd.c_str()) != 0) {
        std::cerr << "Failed to set channel " << channel << std::endl;
    }
}

int main() {
    if (geteuid() != 0) {
        std::cerr << "This program must be run as root!" << std::endl;
        return 1;
    }

    std::string iface = "wlp0s20f0u5u3";

    if (!isMonitorMode(iface)) {
        setMonitorMode(iface);
        if (!isMonitorMode(iface)) {
            std::cerr << "Failed to put network card into monitor mode" << std::endl;
            return 1;
        }
    }

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
            setChannel(iface, ch);
            std::cout << "Switched to channel " << ch << std::endl;

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
    return 0;
}
