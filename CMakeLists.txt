cmake_minimum_required(VERSION 3.20)
project(Stellarium VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

# Generate a compile_commands.json file for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the project name
set(PROJECT_NAME "Stellarium")

# Specify the source files
file(GLOB SRC_FILES "src/*.cpp" "src/*.cc")

# Specify the include directories
set(INCLUDE_DIRS "src")

# Add the library target
add_library(${PROJECT_NAME} ${SRC_FILES})

# Compiler flags
target_compile_options(${PROJECT_NAME} PUBLIC -g -Wall -Wextra -Wpedantic -O3)

# Set include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

# Compile all tests
add_subdirectory(test)

