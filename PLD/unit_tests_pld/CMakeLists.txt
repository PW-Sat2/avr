add_compile_options(-Wno-error)

add_hal_executable(unit_tests_pld
    main.cpp
    src/commands.cpp
    src/AD7714.cpp
    src/AD7714_driver.cpp
)

add_custom_target(unit_tests_pld.run
    COMMAND ${SIMAVR} -m ${SIMAVR_TARGET} unit_tests_pld.elf 2>&1 | tee ${CMAKE_BINARY_DIR}/unit_tests_pld.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/unit_tests_pld.output && exit 1; || exit 0;
    DEPENDS unit_tests_pld
)

include_directories(.)

target_link_libraries(unit_tests_pld
    unity
    terminal
    ObcInterface
    hardware
    commands
    telemetry
    devices
)
