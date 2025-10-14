#include "GraphicsEngine.h"
#include "Camera.h"
#include "Matrix44.h"
#include "Mesh.h"
#include "Quaternion.h"
#include "Model.h"
#include "Shader.h"
#include "Vector3.h"
#include "Config.h"
#include "import/assimp_importer.h"

#include <filesystem>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace Stellarium
{

/* 
TODO: These global definitions are absolutely heinous.
Glfw doesn't support std::function type callbacks for glfwSetCursorPosCallback
and the like. The result of this is that we cannot use lambda functions and must use 
honest-to-goodness raw function pointers.
*/
GraphicsEngine* _global_graphics_ptr = nullptr;

void _mouse_callback_wrap(GLFWwindow* window, double mouse_x, double mouse_y)
{ 
    _global_graphics_ptr->mouse_callback(window, mouse_x, mouse_y);
}

void _scroll_callback_wrap(GLFWwindow* window, double mouse_x, double mouse_y)
{ 
    _global_graphics_ptr->scroll_callback(window, mouse_x, mouse_y);
}


GraphicsEngine::GraphicsEngine()
{
    _global_graphics_ptr = this;

    // Create main camera
    _camera = std::make_unique<Camera>("main_camera");

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create main window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "stellarium", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // Window callbacks

    // Resize viewport every time window is resized by user
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Mouse input
    glfwSetCursorPosCallback(window, _mouse_callback_wrap);

    // Scroll input
    glfwSetScrollCallback(window, _scroll_callback_wrap);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    Shader shader(
        (std::filesystem::path(STELL_PROJECT_ROOT) / std::filesystem::path("src/render/shader/shader.vert")).string(), 
        (std::filesystem::path(STELL_PROJECT_ROOT) / std::filesystem::path("src/render/shader/shader.frag")).string()
    );

    // Load model
    AssimpImporter importer;
    Model model = importer.loadModel((std::filesystem::path(STELL_PROJECT_ROOT) / std::filesystem::path("assets/backpack/backpack.obj")).string());
    setupModel(model);

    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        _current_frame_time = static_cast<float>(glfwGetTime());
        _delta_frame_time = _current_frame_time - _prev_frame_time;
        _prev_frame_time = _current_frame_time;

        // User input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();

        // Camera projection matrix
        Matrix44 projection = _camera->getProjectionMatrix();
        shader.setMat4("projection", projection);

        // Camera view transformation
        Matrix44 view = _camera->getViewMatrix();
        shader.setMat4("view", view);

        // Render models
        Matrix44 model_matrix = Matrix44();
        shader.setMat4("model", model_matrix);
        drawModel(model, shader);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    // Clean up
    glfwTerminate();

}

void GraphicsEngine::setupModel(Model& model)
{
    for (Mesh& mesh : model.meshes)
    {
        setupMesh(mesh);
    }

    std::cout << "Model loaded successfully from " << model.path.string() << std::endl;

}

void GraphicsEngine::setupMesh(Mesh& mesh)
{
    if (_mesh_buffer_objects.find(&mesh) == _mesh_buffer_objects.end())
    {
        MeshBufferObjectIds ids;
        _mesh_buffer_objects[&mesh] = ids;
    }

    // create buffers/arrays
    glGenVertexArrays(1, &_mesh_buffer_objects[&mesh].VAO);
    glGenBuffers(1, &_mesh_buffer_objects[&mesh].VBO);
    glGenBuffers(1, &_mesh_buffer_objects[&mesh].EBO);

    glBindVertexArray(_mesh_buffer_objects[&mesh].VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, _mesh_buffer_objects[&mesh].VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh_buffer_objects[&mesh].EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    glBindVertexArray(0);

    // load textures
    for (Texture& texture : mesh.textures)
    {
        texture.id = loadTextureFromFile(texture.path);
        // std::cout << "Texture loaded: " << texture.path << " with ID: " << texture.id << std::endl;
    }
}

unsigned int GraphicsEngine::loadTextureFromFile(const std::string& path)
{
    if (_textures.find(path) != _textures.end())
    {
        return _textures[path];
    }
    
    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, num_channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);

    if (!data)
    {
        std::cout << "Failed to load texture at path: " << path << std::endl;
        stbi_image_free(data);
        return -1;
    }

    GLenum format;
    if (num_channels == 1)
    {
        format = GL_RED;
    }
    else if (num_channels == 3)
    {
        format = GL_RGB;
    }
    else if (num_channels == 4)
    {
        format = GL_RGBA;
    }
    else 
    {
        std::cout << "Unsupported number of channels: " << num_channels << std::endl;
        stbi_image_free(data);
        return -1;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << "Texture " << texture_id << " loaded successfully from path: " << path << std::endl;
    std::cout << "Texture size: " << width << "x" << height << ", channels: " << num_channels << std::endl;

    stbi_image_free(data);

    _textures[path] = texture_id;

    return texture_id;
}

void GraphicsEngine::drawModel(const Model& model, const Shader& shader) const
{
    for (const Mesh& mesh : model.meshes)
    {
        drawMesh(mesh, shader);
    }
}

void GraphicsEngine::drawMesh(const Mesh& mesh, const Shader& shader) const
{
    // bind appropriate textures
    unsigned int num_diffuse_textures = 1;
    unsigned int num_specular_textures = 1;
    unsigned int num_normal_textures = 1;
    unsigned int num_height_textures = 1;
    
    for (size_t i = 0; i < mesh.textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = mesh.textures[i].type;
        if (name == "texture_diffuse")
        {
            number = std::to_string(num_diffuse_textures++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(num_specular_textures++);
        }
        else if (name == "texture_normal")
        {
            number = std::to_string(num_normal_textures++);
        }
        else if (name == "texture_height")
        {
            number = std::to_string(num_height_textures++);
        }
        else 
        {
            throw std::runtime_error("Unknown texture type: " + name);
        }

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.getId(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
    
    // draw mesh
    glBindVertexArray(_mesh_buffer_objects.at(&mesh).VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}