file(GLOB_RECURSE TEST_FILES "test_*.cpp")

foreach(TEST_FILE ${TEST_FILES})
    string(REPLACE ".cpp" "" TEST_NAME ${TEST_FILE})
    cmake_path(GET TEST_NAME FILENAME TEST_NAME)
    message(STATUS "Adding test: ${TEST_NAME}")
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_compile_options(${TEST_NAME} PUBLIC -g -Wall -Wextra -Wpedantic -O3)
    target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/build)
    target_link_libraries(${TEST_NAME} PUBLIC Stellarium)
endforeach()

