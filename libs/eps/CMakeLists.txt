if(EPS)
    set(NAME eps)

    add_library(${NAME} STATIC
        obc_interface.cpp
        periodics.cpp
        init.cpp
        eps.h
        defs.h
        calibration.h)

    target_include_directories(${NAME} PUBLIC .)

    target_link_libraries(${NAME}
        ObcInterface
        logger
        eps_commands
        LclCommander
        Prescaler
        ThermalKnives
        PowerCycle
        ObcWatchdog
        OverheatProtection
        BatteryManager
        PowerCycleCounter
        eps_specs
        calibration
    )
endif()