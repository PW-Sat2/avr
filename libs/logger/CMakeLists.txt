set(NAME logger)

add_library(${NAME} STATIC
    logger.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
