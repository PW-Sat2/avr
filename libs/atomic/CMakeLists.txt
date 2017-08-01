set(NAME atomic)

add_library(${NAME} STATIC
    atomic.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
