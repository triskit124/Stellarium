#ifndef STELL_ASSIMP_IMPORTER_H
#define STELL_ASSIMP_IMPORTER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include "Model.h"
#include "Mesh.h"

namespace Stellarium
{

Model loadModelFromAssimp(const std::string& path);
void processNode(aiNode *node, const aiScene *scene, const std::filesystem::path& path, std::vector<Mesh>& meshes);
Mesh processMesh(aiMesh *mesh, const aiScene *scene, const std::filesystem::path& path);
std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, const std::filesystem::path& path);

} // namespace Stellarium

#endif // STELL_ASSIMP_IMPORTER_H