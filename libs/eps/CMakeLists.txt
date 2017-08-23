set(NAME eps)

add_library(${NAME} STATIC
    eps.h
)

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
)
