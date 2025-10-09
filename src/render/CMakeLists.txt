
set(STELL_RENDER_SRCS
    Camera.h
    Camera.cpp
    GraphicsEngine.h
    GraphicsEngine.cpp
    Shader.h
    Shader.cpp
    Window.h
    Window.cpp
    Model.h
    Mesh.h
    import/assimp_importer.h
    import/assimp_importer.cpp
    glad/glad.c
)

target_sources(stellarium PUBLIC ${STELL_RENDER_SRCS})

Include(FetchContent)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        fb375dd8c0a032106a2122815fb18dffe0283721 # 6.0.2
)

FetchContent_MakeAvailable(assimp)

target_link_libraries(
    stellarium 
    PUBLIC 
    glfw3 
    GL 
    X11 
    pthread 
    Xrandr 
    Xi 
    dl
    assimp
)
