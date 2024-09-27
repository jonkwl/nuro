#pragma once

#include <glad/glad.h>

class CubeMesh
{
public:
	CubeMesh();
	
	void bind();
	unsigned int getIndiceCount();
private:
	unsigned int id;
	unsigned int indice_count;
};

