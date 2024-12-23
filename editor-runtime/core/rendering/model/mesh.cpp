#include "mesh.h"

#include <glad/glad.h>

#include "../core/rendering/texture/texture.h"

MeshData::MeshData() : nVertices(0),
nIndices(0),
materialIndex(0)
{
}

MeshData::MeshData(uint32_t nVertices, uint32_t nIndices, uint32_t materialIndex) : nVertices(nVertices),
nIndices(nIndices),
materialIndex(materialIndex)
{
}

Mesh::Mesh() : vao(0),
vbo(0),
ebo(0),
meshData()
{
}

Mesh::Mesh(uint32_t vao, uint32_t vbo, uint32_t ebo, MeshData meshData) : vao(vao),
vbo(vbo),
ebo(ebo),
meshData(meshData)
{
}

Mesh::Mesh(std::vector<VertexData> vertices, std::vector<uint32_t> indices, int32_t materialIndex) : vao(0),
vbo(0),
ebo(0),
meshData(MeshData(vertices.size(), indices.size(), materialIndex))
{
	// Generate VAO, VBO and EBO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind VAO
	glBindVertexArray(vao);

	// Bind VBO, allocate its memory send vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

	// Bind EBO, allocate its memory and send indice data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

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
}

uint32_t Mesh::getVAO() const
{
	return vao;
}

uint32_t Mesh::getVBO() const
{
	return vbo;
}

uint32_t Mesh::getEBO() const
{
	return ebo;
}

uint32_t Mesh::getVerticeCount() const
{
	return meshData.nVertices;
}

uint32_t Mesh::getIndiceCount() const
{
	return meshData.nIndices;
}

uint32_t Mesh::getMaterialIndex() const
{
	return meshData.materialIndex;
}
