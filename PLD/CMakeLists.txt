cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)
set(HAL_MCU ATMEGA324P_44)
set(F_CPU 8000000)
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

add_definitions(-DLOG_LEVEL=LOG_LEVEL_INFO)

project(pld_libs)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(src)
add_subdirectory(pld_libs)
add_subdirectory(unit_tests_pld)
