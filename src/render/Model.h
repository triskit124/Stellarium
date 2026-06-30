#pragma once

#include <vector>
#include <filesystem>

#include "Frame.h"
#include "Mesh.h"
#include "Vector3.h"

namespace Stellarium
{

class Model
{

public:

    // Constructor used by AssimpImporter
    Model(const std::filesystem::path& path, std::vector<Mesh> meshes)
        : path(path), meshes(meshes) {}

    Frame* getFrame() const { return _frame; }
    void setFrame(Frame* frame) { _frame = frame; }

    Vector3 getScale() const { return _scale; }
    void setScale(const Vector3& scale) { _scale = scale; }

    std::filesystem::path path;
    std::vector<Mesh> meshes;

private:

    Frame* _frame = nullptr;
    Vector3 _scale { 1.0, 1.0, 1.0 };

};

} // namespace Stellarium
