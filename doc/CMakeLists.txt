
# Download and include doxygen-awesome-css for Doxygen styling
include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)
 
# Save the location the files were cloned into
# This allows us to get the path to doxygen-awesome.css
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)
 
# Generate the Doxyfile
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Add a custom target to generate the documentation
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/build"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/build"
    COMMAND doxygen "${DOXYFILE_OUT}"
    DEPENDS ${DOXYFILE_OUT}
)
add_custom_target(docs ALL DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/build")
