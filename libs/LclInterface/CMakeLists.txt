set(NAME LclInterface)

add_library(${NAME} STATIC
    LclInterface.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
)
