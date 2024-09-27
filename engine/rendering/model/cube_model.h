#pragma once

#include <glad/glad.h>

class CubeModel
{
public:
	CubeModel();
	
	void bind();
	unsigned int getIndiceCount();
private:
	unsigned int id;
	unsigned int indice_count;
};

