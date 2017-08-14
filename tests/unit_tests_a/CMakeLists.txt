add_hal_executable(unit_tests_a
    main.cpp
    src/Atomic.cpp
    src/CommandDispatcherTest.cpp
    src/Prescaler.cpp
    src/SoftwareTimer.cpp
    src/LclInterface.cpp
    src/LclCommander.cpp
    src/redundancy.cpp
    src/PowerCycleCounter.cpp
    src/calibration.cpp
)

add_custom_target(unit_tests_a.run
    COMMAND ${SIMAVR} -m ${SIMAVR_TARGET} unit_tests_a.elf 2>&1 | tee ${CMAKE_BINARY_DIR}/unit_tests_a.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/unit_tests_a.output && exit 1; || exit 0;
    DEPENDS unit_tests_a
)

include_directories(.)

target_link_libraries(unit_tests_a
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
    calibration
)
