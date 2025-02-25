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

	uint32_t nVertices;
	uint32_t nIndices;
	uint32_t materialIndex;
};