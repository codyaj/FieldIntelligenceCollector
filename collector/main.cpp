#include <pcap.h>
#include <iostream>

int main() {
    // Find devices
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    pcap_t *handle = pcap_open_live(
        "wlp0s20f0u5u3",     // device name
        65535,               // Snapshot length (max num of bytes per packet)
        1,                   // promiscuous mode (1 on, 0 off)
        1000,                // Wait x ms until a packet. (0 = wait forever)
        errbuf
    );
    if (!handle) {
        std::cerr << "Failed to open device: " << errbuf << std::endl;
        return 1;
    }

    std::cout << "Device opened in promiscuous mode: " << "wlp2s0" << std::endl;

    // Optional: set a filter (e.g., all traffic)
    struct bpf_program fp;
    if (pcap_compile(handle, &fp, "", 1, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "Couldn't compile filter: " << pcap_geterr(handle) << std::endl;
    } else {
        pcap_setfilter(handle, &fp);
    }

    // Capture packets
    pcap_loop(handle, 10, [](u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
        std::cout << "Captured packet length: " << header->len << std::endl;
    }, nullptr);

    pcap_close(handle);

    return 0;
}
