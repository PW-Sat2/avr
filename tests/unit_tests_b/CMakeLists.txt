add_hal_executable(unit_tests_b
    main.cpp
    src/BatteryHeater.cpp
    src/BatteryManager.cpp
    src/LCLCommands.cpp
    src/ObcWatchdog.cpp
    src/OverheatProtection.cpp
    src/ThermalKnives.cpp
    src/TwoStateFsm.cpp
    src/DisablePowerCycle.cpp
)

add_custom_target(unit_tests_b.run
    COMMAND ${SIMAVR} -m ${SIMAVR_TARGET} unit_tests_b.elf 2>&1 | tee ${CMAKE_BINARY_DIR}/unit_tests_b.output
    COMMAND grep "FAIL" ${CMAKE_BINARY_DIR}/unit_tests_b.output && exit 1; || exit 0;
    DEPENDS unit_tests_b
)

include_directories(.)

target_link_libraries(unit_tests_b
    unity
    terminal
    BatteryManager
    TwoStateFsm
    OverheatProtection
    eps_commands
    ObcWatchdog
    ThermalKnives
)
