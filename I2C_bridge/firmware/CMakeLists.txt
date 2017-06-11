cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)
set(BOARD ARDUINONANO328P)
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

project(I2C_bridge)

add_subdirectory(${HAL_PATH} hal)

add_hal_executable(I2C_bridge
    main.cpp
)

target_link_libraries(I2C_bridge
    terminal
)