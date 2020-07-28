cmake_minimum_required(VERSION 3.10)
project (mpegts)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
    mpegts
    mpegts/common.cpp
    mpegts/simple_buffer.cpp 
    mpegts/mpegts_muxer.cpp 
    mpegts/mpegts_demuxer.cpp 
    mpegts/ts_packet.cpp 
    mpegts/crc.cpp 
)

target_include_directories(
    mpegts
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(
    mpegts_test
    mpegts/main.cpp
)

target_link_libraries(
    mpegts_test
    mpegts
)

add_executable(
    mpegts_unit_test
    unit_tests/unit_test_1.cpp
    unit_tests/unit_test_2.cpp
    unit_tests/unit_test_3.cpp
    unit_tests/unit_test_4.cpp
    unit_tests/unit_test_5.cpp
    unit_tests/unit_test_6.cpp
    unit_tests/unit_tests.cpp
)

target_link_libraries(
    mpegts_unit_test
    mpegts
)

set_target_properties(mpegts_unit_test PROPERTIES EXCLUDE_FROM_ALL TRUE)
