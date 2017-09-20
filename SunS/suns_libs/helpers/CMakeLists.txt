set(NAME helpers)

add_library(${NAME} STATIC
    Include/helpers/validate_params.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME} hal logger)
