#include "model.h"

#include <vector>
#include <sstream>
#include <filesystem>
#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <rendering/transformation/transformation.h>
#include <utils/console.h>
#include <utils/ioutils.h>
#include <utils/string_helper.h>

namespace fs = std::filesystem;

bool Model::loadIoData()
{
	// Read file
	Assimp::Importer import;
	const uint32_t importSettings = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	const aiScene* scene = import.ReadFile(path, importSettings);

	// Validate model
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Console::out::warning("Model", "Couldn't load model '" + IOUtils::getFilename(path) + "'", import.GetErrorString());
		return false;
	}

	// Reserve materials
	/*modelMaterials.reserve(scene->mNumMaterials);
	for (uint32_t i = 0; i < static_cast<uint32_t>(scene->mNumMaterials); i++)
	{
		modelMaterials.push_back(scene->mMaterials[i]);
	}*/

	// Initiate recursive processing of model
	processNode(scene->mRootNode, scene);

	// Finalize the metrics
	finalizeMetrics();

	return true;
}

void Model::freeIoData()
{
	meshData.clear();
}

bool Model::uploadBuffers()
{
	// Don't dispatch model if there is no data
	if (meshData.empty()) return false;

	// Dispatch each mesh
	for (uint32_t i = 0; i < meshData.size(); i++) {
		// Get mesh data metrics
		uint32_t nVertices = meshData[i].vertices.size();
		uint32_t nIndices = meshData[i].indices.size();
		uint32_t materialIndex = meshData[i].materialIndex;

		// VAO, VBO and EBO backend ids
		uint32_t vao, vbo, ebo;

		// Generate VAO, VBO and EBO
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		// Bind VAO
		glBindVertexArray(vao);

		// Bind VBO, allocate its memory send vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(VertexData), meshData[i].vertices.data(), GL_STATIC_DRAW);

		// Bind EBO, allocate its memory and send indice data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(uint32_t), meshData[i].indices.data(), GL_STATIC_DRAW);

		// Set attributes for VAO
		// Vertex position attribute (location = 0)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
		// Normal attribute (location = 1)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
		// Texture coordinates attribute (location = 2)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
		// Tangent attribute (location = 3)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tangent));
		// Bitangent attribute (location = 3)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, bitangent));

		// Unbind VAO, ABO and EBO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Mesh is not existing yet, create empty mesh
		if (meshes.find(i) == meshes.end()) {
			meshes[i] = Mesh();
		}

		// Update mesh
		meshes[i].setData(vao, vbo, ebo, nVertices, nIndices, materialIndex);
	}

	return true;
}

void Model::deleteBuffers()
{
	for (auto& [id, mesh] : meshes) {
		uint32_t vao = mesh.vao(), vbo = mesh.vbo(), ebo = mesh.ebo();
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
	meshes.clear();
}

Model::Model() : path(),
meshData(),
meshes(),
metrics()
{
}

Model::~Model()
{
	freeIoData();
	deleteBuffers();
}

void Model::setSource(std::string _path)
{
	// Validate source path
	if (!fs::exists(_path))
		Console::out::warning("Model", "Model source at '" + _path + "' could not be found");

	path = _path;
}

const Mesh* Model::queryMesh(uint32_t index)
{
	// Return queried mesh (Creates empty mesh if requested mesh is not existing yet)
	return &meshes[index];
}

uint32_t Model::nLoadedMeshes() const
{
	return meshes.size();
}

Model::Metrics Model::getMetrics() const
{
	return metrics;
}

Mesh* Model::createStaticMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices)
{
	// Get mesh data metrics
	uint32_t nVertices = vertices.size();
	uint32_t nIndices = indices.size();
	uint32_t materialIndex = 0;

	// VAO, VBO and EBO backend ids
	uint32_t vao, vbo, ebo;

	// Generate VAO, VBO and EBO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind VAO
	glBindVertexArray(vao);

	// Bind VBO, allocate its memory send vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

	// Bind EBO, allocate its memory and send indice data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

	// Set attributes for VAO
	// Vertex position attribute (location = 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
	// Normal attribute (location = 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
	// Texture coordinates attribute (location = 2)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
	// Tangent attribute (location = 3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tangent));
	// Bitangent attribute (location = 3)
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, bitangent));

	// Unbind VAO, ABO and EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Create mesh container object
	Mesh* mesh = new Mesh();
	mesh->setData(vao, vbo, ebo, nVertices, nIndices, materialIndex);

	// Return mesh
	return mesh;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		meshData.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
	}
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Model::MeshData Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//
	// INITIALIZE BUFFERS
	// 

	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	uint32_t materialIndex;

	//
	// ADD VERTICES
	//

	vertices.reserve(mesh->mNumVertices);

	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.emplace_back(
			glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z), // POSITION
			glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z), // NORMAL
			mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f), // UV
			glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z), // TANGENT
			glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z) // BITANGENT
		);
	}

	//
	// ADD INDICES
	//

	indices.reserve(mesh->mNumFaces * 3); // Reserve 3 indices per face because each face is a triangle

	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		// Get current face
		const aiFace& face = mesh->mFaces[i];

		// Add indices
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//
	// HANDLE MESH MATERIAL
	//

	// Get meshes material index
	materialIndex = mesh->mMaterialIndex;

	// Implement texture name linking here

	//
	// HANDLE MESH METRICS
	//

	// Update total materials metric if current material index as element count is the highest
	metrics.nMaterials = std::max(metrics.nMaterials, materialIndex + 1);

	// Add mesh to metrics
	addMeshToMetrics(vertices, mesh->mNumFaces);

	// Construct and return mesh data
	return MeshData(std::move(vertices), std::move(indices), materialIndex);
}

void Model::addMeshToMetrics(const std::vector<VertexData>& vertices, uint32_t nFaces)
{
	// Add number of vertices and faces to metrics
	metrics.nVertices += vertices.size();
	metrics.nFaces += nFaces;

	// Loop through all mesh vertices
	for (const VertexData& vertex : vertices)
	{
		// Update min and max point
		metrics.minPoint = glm::min(metrics.minPoint, vertex.position);
		metrics.maxPoint = glm::max(metrics.maxPoint, vertex.position);

		// Add vertex position to centroid
		metrics.centroid += vertex.position;

		// Calculate furthest distance
		metrics.furthest = glm::max(metrics.furthest, glm::distance(glm::vec3(0.0f), vertex.position));
	}
}

void Model::finalizeMetrics()
{
	// Calculate models center and transform to world space
	metrics.origin = (metrics.minPoint + metrics.maxPoint) * 0.5f;
	metrics.origin = Transformation::swap(metrics.origin);

	// Average centroid and transform to world space
	metrics.centroid /= metrics.nVertices;
	metrics.centroid = Transformation::swap(metrics.centroid);
}
