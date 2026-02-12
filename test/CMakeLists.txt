
set(
    STELL_TESTS
    test_math/test_matrix.cpp
    test_math/test_matrix33.cpp
    test_math/test_quaternion.cpp
    test_math/test_transforms.cpp
    test_math/test_vector3.cpp
    test_sim/test_simple_rotation.cpp
    test_sim/test_simple_translation.cpp
)

if (BUILD_RENDERING)
    list(
        APPEND STELL_TESTS
        test_graphics/test_window.cpp
    )
endif()

foreach(TEST_FILE ${STELL_TESTS})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE) # strip off path and extension from filename
    cmake_path(GET TEST_NAME FILENAME TEST_NAME)
    message(STATUS "Adding test: ${TEST_NAME}")
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE stellarium)
endforeach()

