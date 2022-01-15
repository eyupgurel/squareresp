cmake_minimum_required(VERSION 3.21)
project(squareresp)


set(CMAKE_CXX_STANDARD 23)


add_executable(squareresp main.cpp test/churn.cpp test/churn.h)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(rxcpp CONFIG)
include_directories("~/Documents/sc/c++/zeromq/matching.engine" "~/Documents/sc/c++/zeromq/matching_engine_test_utils")

#target_link_libraries(squareresp PRIVATE debug nlohmann_json::nlohmann_json cppzmq ~/lib/matching.engine/debug/libmatching_engine.so ~/lib/matching.engine/test_utils/debug/libmatching_engine_test_utils.so)

target_link_libraries(squareresp PRIVATE optimized nlohmann_json::nlohmann_json cppzmq ~/lib/matching.engine/release/libmatching_engine.so  ~/lib/matching.engine/test_utils/release/libmatching_engine_test_utils.so)




