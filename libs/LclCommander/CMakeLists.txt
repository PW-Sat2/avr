set(NAME LclCommander)

add_library(${NAME} STATIC
    LclCommander.h
)

target_include_directories(${NAME} PUBLIC .)

target_link_libraries(${NAME}
    hal
    LclInterface
)
