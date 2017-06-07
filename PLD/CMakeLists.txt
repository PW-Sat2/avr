cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)
set(BOARD ARDUINONANO328P)
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

project(PLD)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(src)