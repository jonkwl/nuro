#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

struct MeshData {
	MeshData();
	MeshData(uint32_t nVertices, uint32_t nIndices);

	uint32_t nVertices;
	uint32_t nIndices;
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
	Mesh();
	explicit Mesh(std::vector<VertexData> vertices, std::vector<uint32_t> indices, int32_t materialIndex);

	uint32_t getVAO();
	uint32_t getVerticeCount();
	uint32_t getIndiceCount();

	uint32_t getMaterialIndex();

private:
	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;

	MeshData meshData;

	uint32_t materialIndex;
};