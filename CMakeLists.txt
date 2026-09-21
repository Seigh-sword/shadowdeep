cmake_minimum_required(VERSION 3.20)
project(shadowdeep VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SHADOWDEEP_BUILD_TESTS "Build tests" ON)
option(SHADOWDEEP_USE_FTXUI "Use FTXUI if available" OFF)
option(SHADOWDEEP_USE_CURL "Use libcurl for updates if available" ON)
option(SHADOWDEEP_WARNINGS_AS_ERRORS "Warnings as errors" OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    if(SHADOWDEEP_WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SHADOWDEEP_SOURCES
    src/platform/paths.cpp
    src/platform/terminal_backend.cpp
    src/terminal/color.cpp
    src/world/tile.cpp
    src/world/dungeon.cpp
    src/world/region.cpp
    src/world/generator.cpp
    src/world/fov.cpp
    src/combat/damage.cpp
    src/effects/effect.cpp
    src/items/item.cpp
    src/items/inventory.cpp
    src/entities/monster.cpp
    src/entities/player.cpp
    src/save/binary_writer.cpp
    src/save/binary_reader.cpp
    src/save/save_header.cpp
    src/save/entry_metadata.cpp
    src/save/save_manager.cpp
    src/config/config.cpp
    src/tui/screen.cpp
    src/app/cli.cpp
    src/app/app.cpp
    src/game/session.cpp
    src/quests/quest.cpp
    src/npc/npc.cpp
    src/shops/shop.cpp
    src/codex/bestiary.cpp
    src/codex/guide_codex.cpp
    src/update/update_manager.cpp
)

add_library(shadowdeep_lib STATIC ${SHADOWDEEP_SOURCES})
target_include_directories(shadowdeep_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(shadowdeep src/main.cpp)
target_link_libraries(shadowdeep PRIVATE shadowdeep_lib)

if(WIN32)
    target_link_libraries(shadowdeep_lib PUBLIC shlwapi shell32 ole32 winhttp)
    target_compile_definitions(shadowdeep_lib PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_definitions(shadowdeep PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

if(SHADOWDEEP_USE_CURL)
    find_package(CURL QUIET)
    if(CURL_FOUND)
        target_link_libraries(shadowdeep_lib PUBLIC CURL::libcurl)
        target_compile_definitions(shadowdeep_lib PUBLIC SHADOWDEEP_HAS_CURL=1)
        message(STATUS "Found libcurl, enabling HTTPS update support")
    else()
        message(STATUS "libcurl not found, update will use fallback curl binary or WinHTTP")
    endif()
endif()

if(SHADOWDEEP_USE_FTXUI)
    include(FetchContent)
    FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v5.0.0
    )
    FetchContent_MakeAvailable(ftxui)
    target_link_libraries(shadowdeep_lib PUBLIC ftxui::screen ftxui::dom ftxui::component)
    target_compile_definitions(shadowdeep_lib PUBLIC SHADOWDEEP_HAS_FTXUI=1)
endif()

if(SHADOWDEEP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS shadowdeep DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/shadowdeep/data OPTIONAL)
install(FILES LICENSE README.md DESTINATION share/doc/shadowdeep OPTIONAL)
install(FILES AGENTS.md AGENT.md DESTINATION share/doc/shadowdeep OPTIONAL)
install(DIRECTORY DOCS/ DESTINATION share/doc/shadowdeep/DOCS OPTIONAL)

set(CPACK_PACKAGE_NAME "shadowdeep")
set(CPACK_PACKAGE_VERSION "Zv1")
set(CPACK_PACKAGE_VENDOR "Seigh-sword")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A roguelike dungeon crawler")
set(CPACK_PACKAGE_CONTACT "github.com/Seigh-sword")
include(CPack)
