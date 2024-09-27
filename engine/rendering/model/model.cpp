#include "model.h"

Model::Model(std::string path)
{
	resolveModel(path);
    generateVAO();
}

void Model::bind() {
    glBindVertexArray(id);
}

unsigned int Model::getIndiceCount() {
    return indice_count;
}

void Model::resolveModel(std::string path)
{
    Assimp::Importer import;
    const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::printError("Mesh", import.GetErrorString());
        return;
    }

    processNode(scene->mRootNode, scene);

    Log::printProcessInfo(std::to_string(meshes.size()));
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    } 
}

aiMesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    return mesh;
}

void Model::generateVAO()
{
    // Define vertices and indices
    unsigned int vertice_components = 3; // Only position now

    std::vector<float> vertices{
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        0.05f, 1.0f, 1.0f
    };

	std::vector<int> indices{
        0, 1, 2
	};
	indice_count = indices.size();

    // Create vertex array object (VAO)
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer object (VBO) and set data
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attrib pointers of VAO using data from VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create element buffer object (EBO)
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // Unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    id = vao;
}