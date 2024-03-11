cmake_minimum_required(VERSION 3.22)
project(test_utility)

set(CMAKE_CXX_STANDARD 17)

add_library(isocket STATIC source/isocket.cpp)
add_library(usocket STATIC source/usocket.cpp)
add_library(utils   STATIC source/utils.cpp)

add_library(GCI STATIC GCI.cpp)
add_library(RS  STATIC RSXXX_FTDI_Serial.cpp)

add_library(UT  STATIC Utility.cpp)

add_executable(rAtomBTP main.cpp)

target_link_libraries(rAtomBTP isocket)
target_link_libraries(rAtomBTP usocket)
target_link_libraries(rAtomBTP   utils)
target_link_libraries(rAtomBTP     GCI)
target_link_libraries(rAtomBTP      RS)

target_link_libraries(UT utils)
target_link_libraries(UT GCI)
target_link_libraries(UT RS)

target_link_libraries(rAtomBTP UT)