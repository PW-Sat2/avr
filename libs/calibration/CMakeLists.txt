set(NAME calibration)

add_library(${NAME} STATIC
    Include/calibration/TMP121.h
    Include/calibration/LMT87.h
    Include/calibration/Adc.h
    TMP121.cpp
    LMT87.cpp
    Include/calibration/VoltageDivider.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME}
    hal
)
