#include "global_quad.h"

#include <glad/glad.h>

namespace GlobalQuad {

	uint32_t _vbo = 0;
	uint32_t _vao = 0;

	void create()
	{
		float vertices[] =
		{
			1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f,

			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f };

		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);

		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}

	void bind()
	{
		glBindVertexArray(_vao);
	}

	void render()
	{
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	const uint32_t vbo()
	{
		return _vbo;
	}

	const uint32_t vao()
	{
		return _vao;
	}

}