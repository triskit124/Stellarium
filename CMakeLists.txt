######################################################################################
#   ███████ ████████ ███████ ██      ██       █████  ██████  ██ ██    ██ ███    ███ 
#   ██         ██    ██      ██      ██      ██   ██ ██   ██ ██ ██    ██ ████  ████ 
#   ███████    ██    █████   ██      ██      ███████ ██████  ██ ██    ██ ██ ████ ██ 
#        ██    ██    ██      ██      ██      ██   ██ ██   ██ ██ ██    ██ ██  ██  ██ 
#   ███████    ██    ███████ ███████ ███████ ██   ██ ██   ██ ██  ██████  ██      ██ 
######################################################################################

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

project(
    stellarium 
    VERSION 0.2
    DESCRIPTION "A pretty-much-from-scratch physics engine"
    HOMEPAGE_URL "https://github.com/triskit124/Stellarium"
)

enable_language(C)
enable_language(CXX)

# Build options
option(BUILD_RENDERING "Enable building of rendering" ON)
option(BUILD_TESTS "Enable building of tests" ON)

# Generate a compile_commands.json file for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(
    stellarium
    SHARED
)

target_include_directories(
    stellarium 
    PUBLIC 
    src/core
    src/math
    src/render
    src/util
)

add_subdirectory(src/core)
add_subdirectory(src/math)
add_subdirectory(src/util)

if (BUILD_TESTS)
    add_subdirectory(test)
    target_compile_definitions(stellarium PUBLIC STELL_BUILD_TESTS)
endif()

if (BUILD_RENDERING)
    add_subdirectory(src/render)
    target_compile_definitions(stellarium PUBLIC STELL_BUILD_RENDERING)
endif()

target_compile_options(
    stellarium 
    PUBLIC 
    -g 
    -Wall 
    -Wextra 
    -Wpedantic 
    -O3
)

set(STELL_LINK_LIBRARIES "")

target_link_libraries(
    stellarium
    PUBLIC ${STELL_LINK_LIBRARIES}
)
