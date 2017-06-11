add_compile_options(-Wno-error)

add_hal_executable(unit_tests
    main.cpp
    src/CommandDispatcher.cpp
)

add_custom_target(unit_tests.run
    COMMAND ${SIMAVR} -m ${SIMAVR_TARGET} unit_tests.elf 2>&1 | tee ${CMAKE_BINARY_DIR}/unit_tests.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/unit_tests.output && exit 1; || exit 0;
    DEPENDS unit_tests
)

include_directories(.)

target_link_libraries(unit_tests
    unity
    terminal
    ObcInterface
)
