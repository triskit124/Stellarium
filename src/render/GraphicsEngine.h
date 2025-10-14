#ifndef STELL_GRAPHICSENGINE
#define STELL_GRAPHICSENGINE

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>


namespace Stellarium

{

class GraphicsEngine
{

    struct MeshBufferObjectIds
    {
        unsigned int VAO, VBO, EBO;
    };

    public:

        GraphicsEngine();

        static const unsigned int WINDOW_WIDTH = 800;
        static const unsigned int WINDOW_HEIGHT = 600;

        static void framebuffer_size_callback(GLFWwindow* /* window */, int width, int height)
        {
            glViewport(0, 0, width, height);
        };

        void setupModel(Model& model);
        void setupMesh(Mesh& mesh);

        void drawModel(const Model& model, const Shader& shader) const;
        void drawMesh(const Mesh& mesh, const Shader& shader) const;

        unsigned int loadTextureFromFile(const std::string& path);

        void processInput(GLFWwindow *window)
        {
            if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                 _camera->processKeyboardInput(Camera::FORWARD, _delta_frame_time);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                 _camera->processKeyboardInput(Camera::BACKWARD, _delta_frame_time);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                 _camera->processKeyboardInput(Camera::LEFT, _delta_frame_time);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                 _camera->processKeyboardInput(Camera::RIGHT, _delta_frame_time);
        }

        void mouse_callback(GLFWwindow* /* window */, double mouse_x, double mouse_y)
        {
            float delta_mouse_x = mouse_x - _prev_mouse_x;
            float delta_mouse_y = _prev_mouse_y - mouse_y; // reversed since y-coordinates go from bottom to top

             _camera->processMouseMovement(delta_mouse_x, delta_mouse_y);

            _prev_mouse_x = mouse_x;
            _prev_mouse_y = mouse_y;
        }

        void scroll_callback(GLFWwindow* /* window */, double /* mouse_x */, double mouse_y)
        {
            _camera->processMouseScroll(mouse_y);
        }

    protected:

        double _current_frame_time { 0.0 };
        double _prev_frame_time { 0.0 };
        double _delta_frame_time { 0.0 };

        double _prev_mouse_x { 0.0 };
        double _prev_mouse_y { 0.0 };

        std::unique_ptr<Camera> _camera = nullptr;
        std::map<std::string, unsigned int> _textures { };
        std::map<const Mesh*, MeshBufferObjectIds> _mesh_buffer_objects { };

};

}

#endif // STELL_GRAPHICSENGINE