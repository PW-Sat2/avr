set(NAME commands)

add_library(${NAME} STATIC
    Include/commands/HouseKeeping.h
    Include/commands/Photodiodes.h
    Include/commands/PT1000.h
    Include/commands/RadFET.h
    Include/commands/SunSref.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
    hardware
    telemetry
)
