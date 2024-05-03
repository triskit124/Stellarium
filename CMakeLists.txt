cmake_minimum_required(VERSION 3.13)
project(Stellarium VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

set(PROJECT_NAME "Stellarium")

# collect up the source files
file(GLOB SRC_FILES "src/*.cpp")

# Add the executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Compiler flags
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -O3)

# Set include directories
# target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

# Link any libraries or dependencies
# target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
