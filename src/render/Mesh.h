#ifndef STELL_MESH_H
#define STELL_MESH_H

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

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
            : vertices(vertices), indices(indices), textures(textures) { }

        std::vector<Vertex> vertices {};
        std::vector<unsigned int> indices {};
        std::vector<Texture> textures {};

    };

} // namespace Stellarium

#endif