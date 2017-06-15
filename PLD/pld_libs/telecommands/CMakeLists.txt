set(NAME telecommands)

add_library(${NAME} STATIC
    Include/telecommands/HouseKeeping.h
    Include/telecommands/Photodiodes.h
    Include/telecommands/PT1000.h
    Include/telecommands/RadFET.h
    Include/telecommands/SunSref.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
    hardware
    telemetry
)
