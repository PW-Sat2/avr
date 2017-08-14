add_compile_options(-Wno-error)

add_hal_executable(unit_tests
    main.cpp
    src/Atomic.cpp
    src/CommandDispatcherTest.cpp
    src/Prescaler.cpp
    src/SoftwareTimer.cpp
    src/LclInterface.cpp
    src/LclCommander.cpp
    src/redundancy.cpp
    src/PowerCycleCounter.cpp
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
    atomic
    SoftwareTimer
    Prescaler
    LclInterface
    LclCommander
    redundancy
    PowerCycleCounter
)
