#include "quad.h"

#include <glad/glad.h>

unsigned int Quad::vbo = 0;
unsigned int Quad::vao = 0;

void Quad::create()
{
	float vertices[] =
	{
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f };

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Quad::bind()
{
	glBindVertexArray(vao);
}

void Quad::render()
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

unsigned int Quad::getVBO()
{
	return vbo;
}

unsigned int Quad::getVAO()
{
	return vao;
}