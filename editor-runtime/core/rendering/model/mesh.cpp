#include "mesh.h"

Mesh::Mesh(uint32_t vao, uint32_t vbo, uint32_t ebo, uint32_t nVertices, uint32_t nIndices, uint32_t materialIndex) : vao(vao),
vbo(vbo),
ebo(ebo),
nVertices(nVertices),
nIndices(nIndices),
materialIndex(materialIndex)
{
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
	return nVertices;
}

uint32_t Mesh::getIndiceCount() const
{
	return nIndices;
}

uint32_t Mesh::getMaterialIndex() const
{
	return materialIndex;
}
