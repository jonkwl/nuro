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
	Mesh(); // Construct empty mesh
	explicit Mesh(uint32_t vao, uint32_t vbo, uint32_t ebo, MeshInfo meshData); // Construct mesh from existing backend buffers
	explicit Mesh(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices, int32_t materialIndex = 0); // Construct mesh by loading mesh into backend using given data

	uint32_t getVAO() const;
	uint32_t getVBO() const;
	uint32_t getEBO() const;

	uint32_t getVerticeCount() const;
	uint32_t getIndiceCount() const;
	uint32_t getMaterialIndex() const;

private:
	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;

	MeshInfo meshData;
};