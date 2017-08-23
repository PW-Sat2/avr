set(NAME PowerCycle)

add_library(${NAME} STATIC
    PowerCycle.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
)
