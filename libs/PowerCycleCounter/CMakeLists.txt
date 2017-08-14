set(NAME PowerCycleCounter)

add_library(${NAME} STATIC
    PowerCycleCounter.h
    PowerCycleCounter.cpp
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    logger
    EepromCounter
)
