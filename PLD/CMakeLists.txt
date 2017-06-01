cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)

set(BOARD ARDUINONANO328P)

set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

project(SingleDeviceTests)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)

add_hal_executable(PLD
    src/main.cpp
)

target_link_libraries(PLD
    ObcInterface
)

target_include_directories(PLD PUBLIC
)