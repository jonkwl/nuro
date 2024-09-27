#pragma once

#include <glad/glad.h>

class CubeMesh
{
public:
	CubeMesh(unsigned int& indice_count);
	
	void bind();
private:
	unsigned int id;
};

