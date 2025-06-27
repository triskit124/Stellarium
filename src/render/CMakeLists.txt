
set(STELL_RENDER_SRCS
    Camera.h
    Camera.cpp
    GraphicsEngine.h
    GraphicsEngine.cpp
    Shader.h
    Shader.cpp
    Window.h
    Window.cpp
)

target_sources(stellarium PRIVATE ${STELL_RENDER_SRCS})

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
)
