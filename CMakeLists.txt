cmake_minimum_required(VERSION 3.21)
project(squareresp)


set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "/home/egl/Documents/sc/c++/boost_1_77_0")
set(BOOSTROOT "/home/egl/Documents/sc/c++/boost_1_77_0")
find_package(Boost 1.77.0)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif()

add_executable(squareresp main.cpp test/data_stream.cpp test/data_stream.h test/churn.cpp test/churn.h)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(rxcpp CONFIG)
include_directories("~/Documents/sc/c++/zeromq/matching.engine")

#target_link_libraries(squareresp PRIVATE debug nlohmann_json::nlohmann_json cppzmq ~/lib/matching.engine/debug/libmatching_engine.so)

target_link_libraries(squareresp PRIVATE optimized nlohmann_json::nlohmann_json cppzmq ~/lib/matching.engine/release/libmatching_engine.so)




