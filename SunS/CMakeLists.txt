cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)
set(HAL_MCU ATMEGA1284P_44)
set(F_CPU 8000000)
set(AVRDUDE_TOOL arduino CACHE STRING "")
set(AVRDUDE_OPTIONS -P -b19200 CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

add_definitions(-DLOG_LEVEL=LOG_LEVEL_ALL)

project(suns_libs)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(src)
add_subdirectory(suns_libs)
add_subdirectory(unit_tests_suns)