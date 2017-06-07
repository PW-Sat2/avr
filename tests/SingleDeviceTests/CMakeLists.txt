cmake_minimum_required(VERSION 3.2)

project(SingleDeviceTests)

add_hal_executable(SingleDeviceTests
    ObcInterface/ObcInterface.cpp
    main.cpp
)

target_link_libraries(SingleDeviceTests
    unity
    ObcInterface
)
