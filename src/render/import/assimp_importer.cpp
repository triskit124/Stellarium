#include "assimp_importer.h"
#include "Mesh.h"
#include "Model.h"
#include "Vector3.h"

#include <assimp/material.h>

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>

namespace Stellarium
{

Model AssimpImporter::loadModel(const std::string& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("ERROR::ASSIMP:: " + std::string(importer.GetErrorString()));
    }

    std::vector<Mesh> meshes { };

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, path, meshes);

    return Model(path, meshes);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void AssimpImporter::processNode(aiNode *node, const aiScene *scene, const std::filesystem::path& path, std::vector<Mesh>& meshes)
{
    // process each mesh located at the current node
    for (size_t i = 0; i < node->mNumMeshes; ++i)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, path));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (size_t i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene, path, meshes);
    }
}

Mesh AssimpImporter::processMesh(aiMesh *mesh, const aiScene *scene, const std::filesystem::path& path)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // normals
        if (mesh->HasNormals())
        {
            vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            // TODO: a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.tex_coords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.tex_coords = vec2(0.0, 0.0);
        }

        vertices.push_back(vertex);
    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
            // std::cout << "Face " << i << ", index " << j << ": " << face.mIndices[j] << std::endl;
        }
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // Flat diffuse color (Kd) fallback, used by the shader for materials with no diffuse image map.
    aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color);

    // 1. diffuse maps
    loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", path, textures);
    
    // 2. specular maps
    loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", path, textures);
    
    // 3. normal maps
    loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", path, textures);
    
    // 4. height maps
    loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", path, textures);

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures, Vector3(diffuse_color.r, diffuse_color.g, diffuse_color.b));
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
void AssimpImporter::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName, const std::filesystem::path& path, std::vector<Texture>& textures)
{
    for (size_t i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString filename;
        mat->GetTexture(type, i, &filename);
        std::string texture_path = (path.parent_path() / std::filesystem::path(filename.C_Str())).string();
        Texture texture;

        if (_textures_loaded.find(texture_path) != _textures_loaded.end())
        {
            // a texture with the same filepath has already been loaded, continue to next one
            texture = _textures_loaded[texture_path];
        }
        else 
        {
            texture.type = typeName;
            texture.path = texture_path;
            _textures_loaded[texture.path] = texture;
        }
        textures.push_back(texture);
    }
}

} // namespace Stellarium