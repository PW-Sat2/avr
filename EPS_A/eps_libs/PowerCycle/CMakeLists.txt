set(NAME PowerCycle)

add_library(${NAME} STATIC
    Include/PowerCycle/PowerCycle.h
    PowerCycle.cpp
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
    IOMap
)
