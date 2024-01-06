#include "Globals.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"

#define TINYGLTF_IMPLEMENTATION

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

void Importer::Model::Import(const char* filePath)
{
    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;
    std::string error, warning;
    bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, filePath);
    if (!loadOk)
    {
        LOG("[MODEL] Error loading %s: %s", filePath, error.c_str());
    }

    std::string modelName = filePath;
    unsigned pos = modelName.find_last_of('/');
    std::string name = modelName.substr(++pos);
    unsigned dotPos = name.find_last_of('.');
    name = name.substr(0,dotPos);

    for (const auto& srcMesh : model.meshes)
    {
        for (const auto& primitive : srcMesh.primitives)
        {
            ResourceMesh* mesh = new ResourceMesh;
            mesh->mMeshName = name.c_str();
            Importer::Mesh::Import(model, primitive, mesh);
           
            delete mesh;
            mesh = nullptr;
        }
    }

    
}
