set(NAME commands)

add_library(${NAME} STATIC
    commands/PowerCycle.h
    commands/LCL.h
    commands/ThermalKnives.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    ObcInterface
    PowerCycle
)
