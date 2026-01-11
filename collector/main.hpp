#pragma once

#include <cstdlib>
#include <pcap.h>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>   // for std::put_time
#include "include/AdapterManager.hpp"
#include "include/Database.hpp"
#include "include/OUIManager.hpp"
#include "include/PacketScanner.hpp"
#include "include/GPSManager.hpp"