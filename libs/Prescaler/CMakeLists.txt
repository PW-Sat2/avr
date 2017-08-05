set(NAME Prescaler)

add_library(${NAME} STATIC
    Prescaler.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
