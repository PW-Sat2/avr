set(NAME MainTimer)

add_library(${NAME} STATIC
    MainTimer.cpp
    MainTimer.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
)
