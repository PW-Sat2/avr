# Download and unpack unity at configure time
configure_file(CMakeLists.txt.in ${CMAKE_CURRENT_BINARY_DIR}/unity/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unity
)

execute_process(COMMAND ${CMAKE_COMMAND} --build .  
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unity
)

set(UNITY_PATH ${CMAKE_CURRENT_BINARY_DIR}/unity-src)

add_library(unity
    ${UNITY_PATH}/src/unity.c
)

target_include_directories(unity PUBLIC
    ${UNITY_PATH}/src
)
