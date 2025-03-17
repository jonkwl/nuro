#include "mesh.h"

Mesh::Mesh() : _vao(0),
_vbo(0),
_ebo(0),
_nVertices(0),
_nIndices(0),
_materialIndex(0)
{
}

void Mesh::setData(uint32_t _vao, uint32_t _vbo, uint32_t _ebo, uint32_t _nVertices, uint32_t _nIndices, uint32_t _materialIndex)
{
	this->_vao = _vao;
	this->_vbo = _vbo;
	this->_ebo = _ebo;
	this->_nVertices = _nVertices;
	this->_nIndices = _nIndices;
	this->_materialIndex = _materialIndex;
}

uint32_t Mesh::vao() const
{
	return _vao;
}

uint32_t Mesh::vbo() const
{
	return _vbo;
}

uint32_t Mesh::ebo() const
{
	return _ebo;
}

uint32_t Mesh::verticeCount() const
{
	return _nVertices;
}

uint32_t Mesh::indiceCount() const
{
	return _nIndices;
}

uint32_t Mesh::materialIndex() const
{
	return _materialIndex;
}