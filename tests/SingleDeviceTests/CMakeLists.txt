cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../../AVR-HAL)

set(BOARD ARDUINONANO328P)

set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

project(SingleDeviceTests)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../../libs libs)
add_subdirectory(unity)

add_hal_executable(SingleDeviceTests
    drivers/ObcInterface.cpp
    main.cpp
)

target_link_libraries(SingleDeviceTests
    unity
    ObcInterface
)
