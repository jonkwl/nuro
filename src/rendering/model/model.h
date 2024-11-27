#pragma once

#include <string>
#include <glm.hpp>
#include <assimp/scene.h>

class Mesh;

struct ModelMetrics
{
	unsigned int nMeshes = 0;	 // Total number of meshes in model
	unsigned int nFaces = 0;	 // Total number of polygon faces in model
	unsigned int nVertices = 0;	 // Total number of vertices in model
	unsigned int nMaterials = 0; // Total number of materials in model

	glm::vec3 minPoint = glm::vec3(0.0f); // Minimum vertex position (bounding box)
	glm::vec3 maxPoint = glm::vec3(0.0f); // Maximum vertex position (bounding box)
	glm::vec3 origin = glm::vec3(0.0f);	  // Center of model (midpoint of min/max points)
	glm::vec3 centroid = glm::vec3(0.0f); // Geometric center (average of all vertices)
	float furthest = 0.0f;				  // Maximum distance of a vertice from object space center
};

class Model
{
public:
	Model(std::string path);
	std::vector<Mesh *> meshes;

	bool castsShadow = true;

	ModelMetrics getMetrics() const; // Returns the model metrics
private:
	std::string directory;

	void resolveModel(std::string path);

	void processNode(aiNode *node, const aiScene *scene);
	Mesh *processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<aiMaterial *> model_materials;

	// Model metrics
	void calculateModelMetrics();
	ModelMetrics metrics;
};