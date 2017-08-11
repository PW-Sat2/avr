set(NAME TwoStateFsm)

add_library(${NAME} STATIC
    TwoStateFsm.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
