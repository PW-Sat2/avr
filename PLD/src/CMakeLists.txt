add_subdirectory(telecommands)
add_subdirectory(telemetry)

add_hal_executable(PLD
    main.cpp
)

target_link_libraries(PLD
    telecommands
    telemetry
)

