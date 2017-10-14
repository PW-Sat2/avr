cmake_minimum_required(VERSION 3.2)

set(EPS TRUE)

set(HAL_PATH ../AVR-HAL)
set(HAL_MCU ATMEGA128A_TQFP64)
set(F_CPU 8000000)

set(AVRDUDE_TOOL dragon_jtag)
set(AVRDUDE_OPTIONS -x jtagchain=0,1,0,4)
set(LOAD_EEPROM TRUE CACHE BOOL "")

set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

add_definitions(-DLOG_LEVEL=LOG_LEVEL_DEBUG)

include(../cmake-modules/nameplate.cmake)

project(eps_a)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(src)
add_subdirectory(eps_libs)

add_subdirectory(eps_unit_tests)
