#include "mesh.h"

Mesh::Mesh() : vao(0),
vbo(0),
ebo(0),
nVertices(0),
nIndices(0),
materialIndex(0)
{
}

void Mesh::setData(uint32_t _vao, uint32_t _vbo, uint32_t _ebo, uint32_t _nVertices, uint32_t _nIndices, uint32_t _materialIndex)
{
	vao = _vao;
	vbo = _vbo;
	ebo = _ebo;
	nVertices = _nVertices;
	nIndices = _nIndices;
	materialIndex = _materialIndex;
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
