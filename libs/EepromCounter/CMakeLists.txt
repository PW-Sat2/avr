set(NAME EepromCounter)

add_library(${NAME} STATIC
    EepromCounter.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    logger
    redundancy
)
