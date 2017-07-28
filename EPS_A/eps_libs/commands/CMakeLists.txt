set(NAME commands)

add_library(${NAME} STATIC
    commands/PowerCycle.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
)
