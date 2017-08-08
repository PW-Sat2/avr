set(NAME redundancy)

add_library(${NAME} STATIC
    redundancy.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
