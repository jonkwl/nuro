#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh();

	// Sets the meshes existing backend buffers and metrics
	void setData(uint32_t vao, uint32_t vbo, uint32_t ebo, uint32_t nVertices, uint32_t nIndices, uint32_t materialIndex);
	
	// Returns the meshes vertex array object
	uint32_t vao() const;

	// Returns the meshes vertex buffer object
	uint32_t vbo() const;

	// Return the meshes element buffer object
	uint32_t ebo() const;

	// Returns the meshes amount of vertices
	uint32_t verticeCount() const;

	// Returns the meshes amount of indices
	uint32_t indiceCount() const;

	// Returns the meshes material index related to the parent model
	uint32_t materialIndex() const;

private:
	uint32_t _vao;
	uint32_t _vbo;
	uint32_t _ebo;

	uint32_t _nVertices;
	uint32_t _nIndices;
	uint32_t _materialIndex;
};