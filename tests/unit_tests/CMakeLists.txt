add_compile_options(-Wno-error)

add_hal_executable(unit_tests
    main.cpp
    src/CommandDispatcher.cpp
)

add_custom_target(unit_tests.run
    COMMAND ${SIMULAVR} -d ${SIMULAVR_TARGET} -f unit_tests.elf -W 0xC6,- -R 0x22,- -T exit | tee ${CMAKE_BINARY_DIR}/unit_tests.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/unit_tests.output && exit 1; || exit 0;
    DEPENDS unit_tests
)

include_directories(.)

target_link_libraries(unit_tests
    unity
    terminal
    ObcInterface
)
