set(NAME TelemetryBuffer)

add_library(${NAME} STATIC
    Include/TelemetryBuffer/TelemetryBuffer.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME} hal gsl)
