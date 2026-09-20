cmake_minimum_required(VERSION 3.20)

add_executable(shadowdeep_tests
    test_main.cpp
    test_rng.cpp
    test_dungeon.cpp
    test_save.cpp
    test_paths.cpp
    test_combat.cpp
)

target_link_libraries(shadowdeep_tests PRIVATE shadowdeep_lib)

add_test(NAME shadowdeep_tests COMMAND shadowdeep_tests)
