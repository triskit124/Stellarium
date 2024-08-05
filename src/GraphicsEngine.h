#ifndef STELL_GRAPHICSENGINE
#define STELL_GRAPHICSENGINE

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Stellarium

{

class GraphicsEngine
{
    public:

        GraphicsEngine();

    protected:
        static void framebuffer_size_callback(GLFWwindow* /* window */, int width, int height)
        {
            glViewport(0, 0, width, height);
        };

        static void processInput(GLFWwindow *window)
        {
            if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        }

};

}

#endif // STELL_GRAPHICSENGINE