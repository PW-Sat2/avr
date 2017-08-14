add_definitions(-U__ASSERT_USE_STDERR)
add_compile_options(-Wno-error)

add_subdirectory(unit_tests_a)
add_subdirectory(unit_tests_b)