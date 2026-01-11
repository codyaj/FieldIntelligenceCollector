#include "../include/GPSManager.hpp"

GPSManager::GPSManager(std::string port) {
    serialPort = open(port.c_str(), O_RDWR | O_NOCTTY); // Open for Read/Write

    if (serialPort < 0) {
        perror("Error opening serial port");
        return;
    }

    struct termios tty;
    if (tcgetattr(serialPort, &tty) != 0) {
        perror("Error from tcgetattr");
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;        // No parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;            // 8 bits
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

    // RAW MODE
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); 
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

    // Timeout settings
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; // 1 second timeout

    tcsetattr(serialPort, TCSANOW, &tty);
}

GPSManager::~GPSManager() {
    close(serialPort);
}

void GPSManager::parse_line(const std::string& line) {
    if (line.find("$GPRMC") != std::string::npos) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // tokens[2] = 'A' for valid, 'V' for invalid
        if (tokens.size() > 6 && tokens[2] == "A") {
            currentData.hasFix = true;
            currentData.latitude = convert_to_decimal(tokens[3], tokens[4]);
            currentData.longitude = convert_to_decimal(tokens[5], tokens[6]);
            
            std::cout << "Fix: " << currentData.latitude << ", " << currentData.longitude << std::endl;
        } else {
            currentData.hasFix = false;
            std::cout << "Waiting for u-blox 7 fix..." << std::endl;
        }
    }
}

double GPSManager::convert_to_decimal(std::string val, std::string hemi) {
    if (val.empty()) return 0.0;
    double raw = std::stod(val);
    int degrees = (int)(raw / 100);
    double minutes = raw - (degrees * 100);
    double decimal = degrees + (minutes / 60.0);
    if (hemi == "S" || hemi == "W") decimal *= -1.0;
    return decimal;
}

void GPSManager::read_data() {
    char charBuf[256];
    int n = read(serialPort, &charBuf, sizeof(charBuf));
    
    if (n > 0) {
        for (int i = 0; i < n; ++i) {
            if (charBuf[i] == '\r' || charBuf[i] == '\n') {
                if (!lineBuffer.empty()) {
                    parse_line(lineBuffer);
                    lineBuffer.clear();
                }
            } else {
                lineBuffer += charBuf[i];
            }
        }
    }
}

const GNSSData& GPSManager::get_current_data() {
    return currentData;
}