#pragma once

#include "Camera.h"
#include "Frame.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <atomic>


namespace Stellarium

{

class GraphicsEngine
{

    struct MeshBufferObjectIds
    {
        unsigned int VAO = -1, VBO = -1, EBO = -1;
    };

    public:

        GraphicsEngine();
        ~GraphicsEngine();

        static const unsigned int WINDOW_WIDTH = 800;
        static const unsigned int WINDOW_HEIGHT = 600;

        static void framebuffer_size_callback(GLFWwindow* /* window */, int width, int height)
        {
            glViewport(0, 0, width, height);
        };

        // Loads a model from a file, associates it with a frame, uploads GPU resources, and stores
        // the model. Returns a raw pointer to the stored model. Must be called before run().
        Model* loadModel(const std::string& path, Frame& frame);

        // Run the render loop. Blocks until the window is closed or requestStop() is called.
        // Must be called from the main thread.
        void run();

        // Mutex that must be held when reading or writing Frame pose data.
        // The physics thread holds it during each _step(); the render loop holds it
        // while snapshotting transforms.
        std::mutex& poseMutex() { return _pose_mutex; }

        void setupModel(Model& model);
        void setupMesh(Mesh& mesh);

        void drawModel(const Model& model, const Shader& shader) const;
        void drawMesh(const Mesh& mesh, const Shader& shader) const;

        // Builds a single large quad on the world XY plane (Z=0) and uploads it to the GPU.
        // The quad itself is a normal perspective-projected mesh; the grid.frag shader draws
        // the actual line pattern procedurally with screen-space-derivative anti-aliasing, so
        // line width stays ~1px regardless of camera distance. Must be called before run().
        void createGrid(double extent = 500.0);

        // Draws the ground grid with alpha blending (for anti-aliased/fading lines) and
        // depth writes disabled, so it composites correctly against already-drawn models.
        void drawGrid() const;

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

        // Blender-style navigation: drag with the middle mouse button to orbit, hold Shift
        // while dragging with the middle mouse button to pan. The cursor stays visible and
        // free otherwise, so drags only apply between a button press and release.
        void mouse_button_callback(GLFWwindow* window, int button, int action, int /* mods */)
        {
            if (button != GLFW_MOUSE_BUTTON_MIDDLE)
                return;

            if (action == GLFW_PRESS)
            {
                bool shift_held = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
                                || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                _panning = shift_held;
                _orbiting = !shift_held;

                // Sync the drag origin to the cursor's current position so the first
                // mouse_callback delta of this drag doesn't jump from a stale position.
                glfwGetCursorPos(window, &_prev_mouse_x, &_prev_mouse_y);
            }
            else if (action == GLFW_RELEASE)
            {
                _panning = false;
                _orbiting = false;
            }
        }

        void mouse_callback(GLFWwindow* /* window */, double mouse_x, double mouse_y)
        {
            double delta_mouse_x = mouse_x - _prev_mouse_x;
            double delta_mouse_y = _prev_mouse_y - mouse_y; // reversed since y-coordinates go from bottom to top

            if (_orbiting)
                _camera->orbit(delta_mouse_x, delta_mouse_y);
            else if (_panning)
                _camera->pan(delta_mouse_x, -delta_mouse_y);

            _prev_mouse_x = mouse_x;
            _prev_mouse_y = mouse_y;
        }

        void scroll_callback(GLFWwindow* /* window */, double /* mouse_x */, double mouse_y)
        {
            _camera->zoom(mouse_y);
        }

        void stopRendering() { this->_should_render.store(false); };
        bool shouldRender() { return this->_should_render.load(); };

    protected:

        double _current_frame_time { 0.0 };
        double _prev_frame_time { 0.0 };
        double _delta_frame_time { 0.0 };

        double _prev_mouse_x { 0.0 };
        double _prev_mouse_y { 0.0 };
        bool _orbiting { false };
        bool _panning { false };

        GLFWwindow* _window = nullptr;
        std::unique_ptr<Camera> _camera = nullptr;
        std::unique_ptr<Shader> _shader;
        std::map<std::string, unsigned int> _textures { };
        std::map<const Mesh*, MeshBufferObjectIds> _mesh_buffer_objects { };

        std::vector<std::unique_ptr<Model>> _models;

        // Ground grid. Kept separate from _models since it uses its own shader and draw
        // state (alpha blending, no depth write) rather than the standard textured-mesh path.
        std::unique_ptr<Shader> _grid_shader;
        std::unique_ptr<Model> _grid_model;
        Frame _grid_frame { "world_grid" }; // stays at the identity pose (world origin)

        std::atomic<bool> _should_render { true };

        std::mutex _pose_mutex;
};

}
