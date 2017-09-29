set(NAME BatteryManager)

add_library(${NAME} STATIC
    BatteryManager.h
    BatteryHeater.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    TwoStateFsm
    logger
)
