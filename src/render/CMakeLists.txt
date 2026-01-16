
set(STELL_RENDER_SRCS
    Camera.cpp
    GraphicsEngine.cpp
    Shader.cpp
    Window.cpp
    import/assimp_importer.cpp
)

target_sources(stellarium PUBLIC ${STELL_RENDER_SRCS})

Include(FetchContent)

# Required dependency: assimp
# ===========================
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        fb375dd8c0a032106a2122815fb18dffe0283721 # 6.0.2
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(assimp)

# Required dependency: stb_image.h
# =================================
FetchContent_Declare(
  stb_image
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(stb_image)

# TODO: Need to explicitly add the stb_image src directory to include path
target_include_directories(stellarium PUBLIC
    "${stb_image_SOURCE_DIR}"
)

# Required dependency: glfw
# =========================
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
  GIT_SHALLOW TRUE
)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_LIBRARY_TYPE SHARED)

FetchContent_MakeAvailable(glfw)

# TODO: Need to explicitly add the glfw src directory to include path
target_include_directories(stellarium PUBLIC
    "${glfw_SOURCE_DIR}/include"
)

# Required dependency: glad
# =========================
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        1ecd45775d96f35170458e6b148eb0708967e402 # 0.1.36
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glad)


if (APPLE)
    set(GRAPHICS_LIBS glfw glad "-framework OpenGL" "-framework Cocoa" assimp)
else()
    set(GRAPHICS_LIBS glfw3 glad GL X11 pthread Xrandr Xi dl assimp)
endif()

target_link_libraries(
    stellarium 
    PUBLIC 
    ${GRAPHICS_LIBS}
)