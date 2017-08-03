set(NAME SoftwareTimer)

add_library(${NAME} STATIC
    SoftwareTimer.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME} hal)
