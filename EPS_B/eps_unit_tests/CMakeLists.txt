add_compile_options(-Wno-error)

add_hal_executable(eps_unit_tests
    main.cpp
    src/TelemetryUpdater.cpp
)

add_custom_target(eps_unit_tests.run
    COMMAND ${SIMAVR} -m ${SIMAVR_TARGET} eps_unit_tests.elf 2>&1 | tee ${CMAKE_BINARY_DIR}/eps_unit_tests.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/eps_unit_tests.output && exit 1; || exit 0;
    DEPENDS eps_unit_tests
)

include_directories(.)

target_link_libraries(eps_unit_tests
    unity
    terminal
    TelemetryUpdater
)
