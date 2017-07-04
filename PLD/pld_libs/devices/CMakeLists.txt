add_library(drivers STATIC
    Include/AD7714.h
)

target_include_directories(drivers PUBLIC Include)

target_link_libraries(drivers
    hal
)
