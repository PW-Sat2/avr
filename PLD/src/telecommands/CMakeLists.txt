set(NAME telecommands)

add_library(${NAME} STATIC
    Include/telecommands/RadFET.h
    Include/telecommands/SunSref.h
)

target_include_directories(${NAME} PUBLIC Include Include)

target_link_libraries(${NAME}
    hal
    telemetry
    ObcInterface
)
