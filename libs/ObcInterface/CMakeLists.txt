set(NAME ObcInterface)

add_library(${NAME} STATIC
    Include/ObcInterface/ObcInterface.h
)

target_include_directories(${NAME} PUBLIC Include)

target_link_libraries(${NAME} hal)
