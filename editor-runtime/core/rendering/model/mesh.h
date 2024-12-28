#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

struct MeshInfo {
	MeshInfo();
	MeshInfo(uint32_t nVertices, uint32_t nIndices, uint32_t materialIndex);

	uint32_t nVertices;
	uint32_t nIndices;

	uint32_t materialIndex;
};

class Mesh
{
public:
	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

public:
	// Constructs empty mesh
	Mesh();

	// Constructs mesh from existing backend buffers
	explicit Mesh(uint32_t vao, uint32_t vbo, uint32_t ebo, MeshInfo meshInfo);

	// Constructs mesh by loading mesh into backend using given data
	explicit Mesh(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices, int32_t materialIndex = 0);

	// Destroys the mesh by unloading all of its data
	void destroy();
	
	// Returns the meshes vertex array object
	uint32_t getVAO() const;

	// Returns the meshes vertex buffer object
	uint32_t getVBO() const;

	// Return the meshes element buffer object
	uint32_t getEBO() const;

	// Returns the meshes amount of vertices
	uint32_t getVerticeCount() const;

	// Returns the meshes amount of indices
	uint32_t getIndiceCount() const;

	// Returns the meshes material index related to the parent model
	uint32_t getMaterialIndex() const;

private:
	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;

	MeshInfo meshInfo;
};