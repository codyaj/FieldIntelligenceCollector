#include "./PacketScanner.hpp"

std::unordered_map<uint8_t, std::unordered_map<uint8_t, std::string>> frameTypes = {
    {0x00, {
        {0x00, "Association Request"},
        {0x01, "Association Response"},
        {0x02, "Reassociation Request"},
        {0x03, "Reassociation Response"},
        {0x04, "Probe Request"},
        {0x05, "Probe Response"},
        {0x06, "Timing Advertisement"},
        {0x08, "Beacon"},
        {0x09, "ATIM"},
        {0x0A, "Disassociation"},
        {0x0B, "Authentication"},
        {0x0C, "Deauthentication"},
        {0x0D, "Action"},
        {0x0E, "Action No Ack (NACK)"},
        }},
    {0x01, {
        {0x02, "Trigger"},
        {0x03, "TACK"},
        {0x04, "Beamforming Report Poll"},
        {0x05, "VHT/HE NDP Announcement"},
        {0x06, "Control Frame Extension"},
        {0x07, "Control Wrapper"},
        {0x08, "Block Ack Request"},
        {0x09, "Block Ack"},
        {0x0A, "PS-Poll"},
        {0x0B, "RTS"},
        {0x0C, "CTS"},
        {0x0D, "ACK"},
        {0x0E, "CF-End"},
        {0x0F, "CF-End + CF-ACK"}
        }},
    {0x02, {
        {0x00, "Data"},
        {0x04, "Null (no data)"},
        {0x08, "QoS Data"},
        {0x09, "QoS Data + CF-ACK"},
        {0x0A, "QoS Data + CF-Poll"},
        {0x0B, "QoS Data + CF-ACK + CF-Poll"},
        {0x0C, "QoS Null (no data)"},
        {0x0E, "QoS CF-Poll (no data)"},
        {0x0F, "QoS CF-ACK + CF-Poll (no data)"}
        }},
    {0x03, {
        {0x00, "DMG Beacon"},
        {0x01, "S1G Beacon"},
        }}
};

namespace {
    std::string mac_to_string(const uint8_t* mac) {
        std::ostringstream oss;
        oss << std::hex << std::uppercase;
        for (int i = 0; i < 6; ++i) {
            if (i != 0) oss << ":";
            oss << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
        }
        return oss.str();
    }

    std::string get_frame_type_name(uint8_t type, uint8_t subtype) {
        auto typeIt = frameTypes.find(type);
        if (typeIt != frameTypes.end()) {
            auto& subtypeMap = typeIt->second;
            auto subIt = subtypeMap.find(subtype);
            if (subIt != subtypeMap.end()) {
                return subIt->second;
            }
        }
        return "Unknown/Reserved";
    }
}

Packet PacketScanner::process_packet(const uint8_t* packetData, size_t length, const std::string& timestamp, int currChannel) {
    Packet pkt;
    pkt.timestamp = timestamp;
    pkt.payload_size = static_cast<int>(length);

    if (length < sizeof(radiotap_header)) { // min header for 802.11
        pkt.protocol = "Invalid";
        return pkt;
    }

    const radiotap_header* rt = reinterpret_cast<const radiotap_header*>(packetData);
    uint16_t rt_len = le16toh(rt->it_len);

    if (length < rt_len + 24) { // need at least radiotap + 802.11 header
        pkt.protocol = "Invalid";
        return pkt;
    }

    // Skip radiotap
    const uint8_t* dot11 = packetData + rt_len;

    // Frame control field: first 2 bytes
    uint16_t frameControl = dot11[0] | (dot11[1] << 8);
    uint8_t type = (frameControl >> 2) & 0x3;      // 2 bits: type
    uint8_t subtype = (frameControl >> 4) & 0xF;   // 4 bits: subtype

    // For now, we label protocol by type
    pkt.protocol = get_frame_type_name(type, subtype);

    // Addresses in 802.11 header
    const uint8_t* addr1 = dot11 + 4;  // Destination
    const uint8_t* addr2 = dot11 + 10; // Source
    const uint8_t* addr3 = dot11 + 16; // BSSID

    pkt.dest_mac = mac_to_string(addr1);
    pkt.source_mac = mac_to_string(addr2);
    pkt.bssid = mac_to_string(addr3);

    pkt.latitude = 0; // dummy
    pkt.longitude = 0; // dummy

    pkt.channel = currChannel;

    return pkt;
}

bool PacketScanner::is_batch_ready() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBatchTime);

    return !batchBuffer.empty() && 
           (batchBuffer.size() >= batchSizeThreshold || 
            elapsed >= batchTimeThreshold);
}

std::vector<Packet> PacketScanner::get_batch() {
    lastBatchTime = std::chrono::steady_clock::now();
    std::vector<Packet> tempBuffer = batchBuffer;
    batchBuffer.clear();
    return tempBuffer;
}