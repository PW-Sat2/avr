set(NAME IOMap)

add_library(${NAME} STATIC
    IOMap.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
)
