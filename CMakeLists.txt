cmake_minimum_required(VERSION 3.24)
project("Wake-On-Lan")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

add_subdirectory(wol-backend/)
add_subdirectory(client/)