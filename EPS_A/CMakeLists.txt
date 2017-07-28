cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)
set(HAL_MCU ATMEGA128A_TQFP64)
set(F_CPU 8000000)
set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

add_definitions(-DLOG_LEVEL=LOG_LEVEL_INFO)

project(eps_a)

add_compile_options(-g)
add_definitions(-D__ASSERT_USE_STDERR)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(eps_libs)
add_subdirectory(src)