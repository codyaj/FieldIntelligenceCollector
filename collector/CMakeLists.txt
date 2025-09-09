cmake_minimum_required(VERSION 3.20)
project(FieldCollector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(include)

# Source files
set(SOURCES
    src/AdapterManager.cpp
    src/PacketScanner.cpp
    src/Database.cpp
    src/OUIManager.cpp
    src/Utils.cpp
    main.cpp
)

# Find libpcap
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Build executable
add_executable(FieldCollector ${SOURCES})

# Link libraries
target_include_directories(FieldCollector PRIVATE ${PCAP_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(FieldCollector PRIVATE ${PCAP_LIBRARIES} ${SQLITE3_LIBRARIES})