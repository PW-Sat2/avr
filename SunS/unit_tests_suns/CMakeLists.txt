add_compile_options(-Wno-error)

add_hal_executable(unit_tests_suns
    main.cpp
    src/telecommands.cpp
)

add_custom_target(unit_tests_suns.run
    COMMAND ${SIMAVR} -m ${SIMAVR_TARGET} unit_tests_suns.elf 2>&1 | tee ${CMAKE_BINARY_DIR}/unit_tests_suns.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/unit_tests_suns.output && exit 1; || exit 0;
    DEPENDS unit_tests_suns
)

include_directories(.)

target_link_libraries(unit_tests_suns
    unity
    terminal
    ObcInterface
    hardware
    telecommands
    telemetry
)
