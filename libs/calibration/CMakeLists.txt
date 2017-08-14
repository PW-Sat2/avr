set(NAME calibration)

add_library(${NAME} STATIC
    Include/calibration/TMP121.h
    TMP121.cpp
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
)
