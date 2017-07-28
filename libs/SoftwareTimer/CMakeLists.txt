set(NAME SoftTimer)

add_library(${NAME} STATIC
    Include/SoftwareTimer.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME} hal logger)
