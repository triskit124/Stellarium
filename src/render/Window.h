#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Stellarium {

class Window {
    public:
        Window(const unsigned int width, const unsigned int height, const std::string& title);
        ~Window();

    };

}