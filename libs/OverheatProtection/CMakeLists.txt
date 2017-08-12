set(NAME OverheatProtection)

add_library(${NAME} STATIC
    OverheatProtection.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
