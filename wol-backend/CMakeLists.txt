add_executable(won src/main.cpp src/ping.cpp src/core.cpp src/icmp_header.cpp src/ipv4_header.cpp src/wol.cpp)

target_include_directories(won PRIVATE include)
