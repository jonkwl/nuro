#include "model.h"

#include <glad/glad.h>
#include <sstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "../src/rendering/core/transformation.h"
#include "../src/utils/log.h"
#include "../src/utils/iohandler.h"
#include "../src/utils/string_helper.h"

Model::Model(std::string path) : castsShadow(true),
meshes(),
directory(""),
modelMaterials(),
metrics()
{
	resolveModel(path);
}

Model::Metrics Model::getMetrics() const
{
	return metrics;
}

void Model::resolveModel(std::string path)
{
	Log::printProcessStart("Model", "Building model " + IOHandler::getFilename(path) + "...");

	// Set model import flags
	unsigned int importSettings = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

	// Read file
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, importSettings);

	// Make sure model is valid
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Log::printError("Mesh", import.GetErrorString());
		return;
	}

	// Add all models materials
	modelMaterials.reserve(scene->mNumMaterials);
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		modelMaterials.push_back(scene->mMaterials[i]);
	}

	// Process each model node, adding each mesh
	processNode(scene->mRootNode, scene);

	// Finalize the metrics
	finalizeMetrics();

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

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	Log::printProcessInfo("- Building mesh " + std::to_string(meshes.size() + 1));

	// Initialize mesh buffers
	std::vector<Mesh::VertexData> vertices;
	std::vector<unsigned int> indices;
	unsigned int materialIndex;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::VertexData vertex;

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
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
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

	// Add mesh to metrics
	addMeshToMetrics(vertices);

	return Mesh(vertices, indices, materialIndex);
}

void Model::addMeshToMetrics(std::vector<Mesh::VertexData> vertices)
{
	// Loop through all mesh vertices
	for (int i = 0; i < vertices.size(); i++)
	{
		// Get current vertex
		Mesh::VertexData vertex = vertices[i];

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
	metrics.origin = Transformation::prepareWorldPosition(metrics.origin);

	// Average centroid and transform to world space
	metrics.centroid /= metrics.nVertices;
	metrics.centroid = Transformation::prepareWorldPosition(metrics.centroid);
}
