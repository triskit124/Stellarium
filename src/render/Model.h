#ifndef STELL_MODEL_H
#define STELL_MODEL_H

#include <vector>
#include <filesystem>

#include "Mesh.h"


namespace Stellarium
{

class Model 
{

public:

    // constructor, expects a filepath to a 3D model.
    Model(std::filesystem::path path, std::vector<Mesh> meshes, std::vector<Texture> textures_loaded) : path(path), meshes(meshes), textures_loaded(textures_loaded) {}

    std::filesystem::path path;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;

};

} // namespace Stellarium

#endif // STELL_MODEL_H