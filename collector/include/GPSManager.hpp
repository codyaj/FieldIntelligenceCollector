#pragma once

#include <iostream>
#include <fcntl.h>    // File Control
#include <termios.h>  // POSIX terminal control
#include <unistd.h>   // UNIX standard function
#include <vector>
#include <string>
#include <sstream>

struct GNSSData {
    double latitude = 0.0;
    double longitude = 0.0;
    double utc = 0.0;
    bool hasFix = false;
};

class GPSManager {
private:
    GNSSData currentData;
    int serialPort;
    std::string lineBuffer;

    double convert_to_decimal(std::string val, std::string hemi);

    void parse_line(const std::string& line);

public:
    GPSManager(std::string port);
    ~GPSManager();

    void read_data();

    const GNSSData& get_current_data();
};