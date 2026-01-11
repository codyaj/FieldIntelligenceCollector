#include "main.hpp"

// Helper to get ISO 8601 timestamp
std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::gmtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%FT%TZ"); // e.g. 2025-09-10T12:34:56Z
    return oss.str();
}

void gnss_fetch_loop(std::shared_ptr<GPSManager> gpsManager) {
    while (true) {
        gpsManager->read_data();
        sleep(1);
    }
}

int main() {

    if (geteuid() != 0) {
        std::cout << "This program must be run as sudo!" << std::endl;
        return 1;
    }

    // Create shared pointer to gpsManager (for use in external thread & packetScanner)
    std::shared_ptr<GPSManager> gpsManager = std::make_shared<GPSManager>("/dev/ttyACM0"); // TODO: Change to singular configuration area

    // Create GNSS thread
    std::thread gnss_thread(gnss_fetch_loop, gpsManager);

    // Create other used classes
    OUIManager ouiManager("../data/oui.csv");  // TODO: Change to singular configuration area
    DatabaseManager databaseManager("../data/data.db");  // TODO: Change to singular configuration area
    std::string iface = "wlan1"; // TODO: Change to singular configuration area
    AdapterManager adapterManager(iface); // TODO: Change to singular configuration area
    PacketScanner packetScanner(gpsManager, 200, 2000); // TODO: Change to singular configuration area

    if (!adapterManager.is_monitor_mode()) {
        std::cout << "Enabling monitor mode" << std::endl;
        if (!adapterManager.set_monitor_mode()) {
            std::cout << "Failed to set to monitor mode" << std::endl;
            return 1;
        }
    }

    // log supported channels
    databaseManager.log_supported_channels(adapterManager.get_supported_channels());

    // Open live capture
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(iface.c_str(), 65535, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "Failed to open device: " << errbuf << std::endl;
        return 1;
    }

    adapterManager.set_channel(36);

    // Allocate capture context ONCE (not every dispatch)
    struct CaptureContext {
        PacketScanner* scanner;
        AdapterManager* adapter;
        OUIManager* ouiManager;
    };

    CaptureContext ctx{ &packetScanner, &adapterManager, &ouiManager };

    while (true) {
        pcap_dispatch(handle, 10,
            [](u_char *user, const struct pcap_pkthdr *header, const u_char *packet) {
                auto* ctx = reinterpret_cast<CaptureContext*>(user);
                PacketScanner* scanner = ctx->scanner;
                AdapterManager* adapter = ctx->adapter;
                OUIManager* oui = ctx->ouiManager;

                int currChannel = adapter->get_current_channel();
                std::string timestamp = get_current_timestamp();

                Packet pkt = scanner->process_packet(
                    packet,
                    header->len,
                    timestamp,
                    currChannel
                );

                if (pkt.protocol == "Invalid") {
                    return; // skip bad packets
                }

                pkt.source_oui_vendor = oui->lookup_MAC(pkt.source_mac);
                pkt.dest_oui_vendor   = oui->lookup_MAC(pkt.dest_mac);

                scanner->add_to_batch(pkt);
            },
            reinterpret_cast<u_char*>(&ctx)  // pass pointer to ctx
        );
        if (packetScanner.is_batch_ready()) {
            databaseManager.insert_packets(packetScanner.get_batch());
            std::cout << "Write complete" << std::endl;
        }
    }

    pcap_close(handle);
    return 0;
}
