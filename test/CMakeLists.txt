file(GLOB_RECURSE TEST_FILES "test*.cpp" "test*.cc")

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE) # strip off path and extension from filename
    cmake_path(GET TEST_NAME FILENAME TEST_NAME)
    message(STATUS "Adding test: ${TEST_NAME}")
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PUBLIC Stellarium)
endforeach()

