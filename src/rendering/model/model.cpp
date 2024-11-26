#include "model.h"

#include "../src/rendering/core/transformation.h"

Model::Model(std::string path)
{
    resolveModel(path);
    calculateModelMetrics();
}

ModelMetrics Model::getMetrics() const
{
    return metrics;
}

void Model::resolveModel(std::string path)
{
    Log::printProcessStart("Model", "Building model " + IOHandler::getFilename(path) + "...");

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

    Log::printProcessDone("Model", "Built model " + IOHandler::getFilename(path));
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

    // Initialize mesh buffers
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

        // Add to total vertices metric
        metrics.nVertices++;
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }

        // Add to total faces metric
        metrics.nFaces++;
    }

    // Texture name linking can be implemented here

    // Get meshes material index
    materialIndex = mesh->mMaterialIndex;

    // Add to total materials metric if current index is the highest index
    metrics.nMaterials = std::max(metrics.nMaterials, materialIndex + 1);
    
    return new Mesh(vertices, indices, textures, materialIndex);
}

void Model::calculateModelMetrics()
{
    // Initialize metrics
    metrics.minPoint = glm::vec3(FLT_MAX);
    metrics.maxPoint = glm::vec3(-FLT_MAX);
    metrics.origin = glm::vec3(0.0f);
    metrics.centroid = glm::vec3(0.0f);
    metrics.furthest = 0.0f;

    // Loop through all meshes
    for (int i = 0; i < meshes.size(); i++) {
        // Get current mesh
        Mesh* mesh = meshes[i];

        // Loop through all mesh vertices
        for (int i = 0; i < mesh->vertices.size(); i++) {
            // Get current vertex
            VertexData vertex = mesh->vertices[i];

            // Update min and max point
            metrics.minPoint = glm::min(metrics.minPoint, vertex.position);
            metrics.maxPoint = glm::max(metrics.maxPoint, vertex.position);

            // Add vertex position to centroid
            metrics.centroid += vertex.position;

            // Calculate furthest distance
            metrics.furthest = glm::max(metrics.furthest, glm::distance(glm::vec3(0.0f), vertex.position));
        }
    }

    // Calculate models center and transform to world space
    metrics.origin = (metrics.minPoint + metrics.maxPoint) * 0.5f;
    metrics.origin = Transformation::prepareWorldPosition(metrics.origin);

    // Average centroid and transform to world space
    metrics.centroid /= metrics.nVertices;
    metrics.centroid = Transformation::prepareWorldPosition(metrics.centroid);
}
