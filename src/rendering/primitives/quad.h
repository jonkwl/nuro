#pragma once

#include <glad/glad.h>

class Quad
{
public:
	static void create();
	static void bind();
	static void render();

	static unsigned int getVBO();
	static unsigned int getVAO();
private:
	static unsigned int vbo;
	static unsigned int vao;
};

