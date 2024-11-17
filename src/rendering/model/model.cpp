#include "model.h"

Model::Model(std::string path)
{
    resolveModel(path);
}

void Model::resolveModel(std::string path)
{
    Log::printProcessStart("Model", "Starting to build model " + IOHandler::GetFilename(path) + "...");

    unsigned int importSettings = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, importSettings);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::printError("Mesh", import.GetErrorString());
        return;
    }

    model_materials.reserve(scene->mNumMaterials);
    for (int i = 0; i < scene->mNumMaterials; i++) {
        model_materials.push_back(scene->mMaterials[i]);
    }

    processNode(scene->mRootNode, scene);

    Log::printProcessDone("Model", "Built model " + IOHandler::GetFilename(path));
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    Log::printProcessInfo("- Building mesh " + std::to_string(meshes.size() + 1));

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures;
    unsigned int materialIndex;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        VertexData vertex;

        // Define unconditional vertex data
        glm::vec3 position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        glm::vec2 uv = glm::vec2(0.0f, 0.0f);
        glm::vec3 tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        glm::vec3 bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

        // Set conditional vertex data
        if (mesh->mTextureCoords[0])
        {
            uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }

        // Assign data to vertex
        vertex.position = position;
        vertex.normal = normal;
        vertex.uv = uv;
        vertex.tangent = tangent;
        vertex.bitangent = bitangent;

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Texture loading can be implemented here

    materialIndex = mesh->mMaterialIndex;
    
    return new Mesh(vertices, indices, textures, materialIndex);
}