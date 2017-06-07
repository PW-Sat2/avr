set(NAME telemetry)

add_library(${NAME} STATIC
    Include/telemetry/telemetry.h
    telemetry.cpp
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME} hal)