cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../../../AVR-HAL)
set(HAL_MCU ATMEGA164P_44)
set(F_CPU 8000000)
set(AVRDUDE_TOOL arduino CACHE STRING "")
set(AVRDUDE_OPTIONS -P -b38400 CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

project(I2C_bridge)

add_subdirectory(${HAL_PATH} hal)

add_hal_executable(I2C_bridge
    main.cpp
)

target_link_libraries(I2C_bridge
    terminal
)

add_definitions("-Wno-error")