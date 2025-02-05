#include "model.h"

#include <vector>
#include <sstream>
#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "../core/rendering/transformation/transformation.h"
#include "../core/utils/console.h"
#include "../core/utils/iohandler.h"
#include "../core/utils/string_helper.h"

Model::Model() : path(),
meshData(),
meshes(),
metrics()
{
}

void Model::setSource(std::string _path)
{
	path = _path;
}

const Mesh* Model::queryMesh(uint32_t index)
{
	// Requested mesh is not existing in meshes (may be loaded later)
	if (index >= meshes.size()) {
		// Resize meshes to contain default mesh at given index
		meshes.resize(index + 1);
	}

	// Return queried mesh
	return &meshes[index];
}

const std::vector<Mesh>& Model::getMeshes()
{
	return meshes;
}

Model::Metrics Model::getMetrics() const
{
	return metrics;
}

std::string Model::sourcePath()
{
	return std::string();
}

void Model::loadData()
{
	// Read file
	Assimp::Importer import;
	const uint32_t importSettings = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	const aiScene* scene = import.ReadFile(path, importSettings);

	// Validate model
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Console::out::warning("Model", "Couldn't load model '" + IOHandler::getFilename(path) + "'", import.GetErrorString());
		return;
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
}

void Model::releaseData()
{
	meshData.clear();
}

void Model::dispatchGPU()
{
	// Don't dispatch model if there is no data
	if (meshData.empty()) return;

	// Resize meshes if it doesn't have enough elements (e.g., from previous queryMesh() calls)
	if (meshes.size() < meshData.size()) {
		meshes.resize(meshData.size());
	}

	// Dispatch each mesh
	for (int i = 0; i < meshData.size(); i++) {
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

		// Update mesh
		meshes[i].setData(vao, vbo, ebo, nVertices, nIndices, materialIndex);
	}
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
	metrics.origin = Transformation::toBackendPosition(metrics.origin);

	// Average centroid and transform to world space
	metrics.centroid /= metrics.nVertices;
	metrics.centroid = Transformation::toBackendPosition(metrics.centroid);
}
