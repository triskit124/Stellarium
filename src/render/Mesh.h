#pragma once

#include <vector>
#include <glad/glad.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Shader.h"


namespace Stellarium {

struct vec3 {
    double x, y, z;
};

struct vec2 {
    double x, y;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 tex_coords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
    public:

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Vector3 diffuse_color = Vector3(1.0, 1.0, 1.0))
            : vertices(vertices), indices(indices), textures(textures), diffuse_color(diffuse_color) { }

        std::vector<Vertex> vertices {};
        std::vector<unsigned int> indices {};
        std::vector<Texture> textures {};
        // Fallback flat color (material's Kd) used when the material has no diffuse texture map.
        Vector3 diffuse_color { 1.0, 1.0, 1.0 };

    };

} // namespace Stellarium
