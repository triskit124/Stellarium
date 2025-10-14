#ifndef STELL_ASSIMP_IMPORTER_H
#define STELL_ASSIMP_IMPORTER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <string>
#include <vector>

#include "Model.h"
#include "Mesh.h"

namespace Stellarium
{

class AssimpImporter
{
    
public:

    AssimpImporter() = default;
    ~AssimpImporter() = default;

    Model loadModel(const std::string& path);

    std::map<std::string, Texture>& getTexturesLoaded() { return _textures_loaded; }

private:
    void processNode(aiNode *node, const aiScene *scene, const std::filesystem::path& path, std::vector<Mesh>& meshes);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene, const std::filesystem::path& path);
    void loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName, const std::filesystem::path& path, std::vector<Texture>& textures);

    std::map<std::string, Texture> _textures_loaded { };
};

} // namespace Stellarium

#endif // STELL_ASSIMP_IMPORTER_H