cmake_minimum_required(VERSION 3.13)
project(Stellarium VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

set(PROJECT_NAME "Stellarium")

# collect up the source files
file(GLOB SRC_FILES "src/*.cpp")

set(INCLUDE_DIRS "build")

# Add the executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Compiler flags
target_compile_options(${PROJECT_NAME} PUBLIC -g -Wall -Wextra -Wpedantic -O3)

# Set include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})


include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG f7320141120f720aecc4c32be25586e7da9eb978 # release tag 0.8.0
)
FetchContent_GetProperties(yaml-cpp)

if(NOT yaml-cpp_POPULATED)
  message(STATUS "Fetching yaml-cpp...")
  FetchContent_Populate(yaml-cpp)
  add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp::yaml-cpp)



# Link any libraries or dependencies
# target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
