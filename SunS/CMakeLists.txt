cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)

set(PLATFORM "FM" CACHE STRING "Model: EM or FM")

if(${PLATFORM} STREQUAL FM)
    set(HAL_MCU ATMEGA324P_44)
elseif(${PLATFORM} STREQUAL EM)
    set(HAL_MCU ATMEGA1284P_44)
else()
    message(FATAL_ERROR "Incorrect platform. Specify EM or FM")
endif()

set(F_CPU 8000000)
set(AVRDUDE_TOOL arduino CACHE STRING "")
set(AVRDUDE_OPTIONS -P -b19200 CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

add_definitions(-DLOG_LEVEL=LOG_LEVEL_WARNING)

project(suns_libs)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(src)
add_subdirectory(suns_libs)
