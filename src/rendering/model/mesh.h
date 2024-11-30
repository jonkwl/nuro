#pragma once

#include <vector>
#include <glm.hpp>

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
	explicit Mesh(std::vector<VertexData> vertices, std::vector<unsigned int> indices, int materialIndex);

	void bind();

	unsigned int getVerticeCount();
	unsigned int getIndiceCount();

	unsigned int getMaterialIndex();

private:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	unsigned int nVertices;
	unsigned int nIndices;

	unsigned int materialIndex;
};