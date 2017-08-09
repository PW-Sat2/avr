cmake_minimum_required(VERSION 3.2)

set(HAL_PATH ../AVR-HAL)
set(HAL_MCU ATMEGA128A_TQFP64)
set(F_CPU 8000000)

set(AVRDUDE_TOOL dragon_jtag)
set(AVRDUDE_OPTIONS -x jtagchain=0,1,0,4)
set(LOAD_EEPROM TRUE CACHE BOOL "")

set(CMAKE_TOOLCHAIN_FILE "${HAL_PATH}/toolchain.cmake")

add_definitions(-DLOG_LEVEL=LOG_LEVEL_INFO)

include(../cmake-modules/GetGitRevisionDescription.cmake)
get_git_head_revision(GIT_BRANCH GIT_REVISION)
git_local_changes(GIT_LOCAL_CHANGES)
string(SUBSTRING ${GIT_REVISION} 0 6 GIT_REVISION_SHORT)
add_definitions(-DGIT_REVISION=\"${GIT_REVISION_SHORT}\")
add_definitions(-DGIT_CHANGES=\"${GIT_LOCAL_CHANGES}\")
message("Revision ${GIT_REVISION_SHORT} ${GIT_LOCAL_CHANGES}")

project(eps_a)

add_compile_options(-g)
add_definitions(-D__ASSERT_USE_STDERR)

add_subdirectory(${HAL_PATH} hal)
add_subdirectory(../libs libs)
add_subdirectory(../tests tests)

add_subdirectory(src)
add_subdirectory(eps_libs)

add_subdirectory(eps_unit_tests)
