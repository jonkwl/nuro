#include "skybox.h"

#include <glad/glad.h>

#include <rendering/skybox/cubemap.h>
#include <rendering/shader/shader_pool.h>
#include <rendering/shader/shader.h>
#include <utils/console.h>

Skybox::Skybox() : cubemap(nullptr),
shader(nullptr),
vao(0),
vbo(0),
emission(1.0f)
{
}

void Skybox::setCubemap(Cubemap* _cubemap)
{
	cubemap = _cubemap;
}

Cubemap* Skybox::getCubemap() const
{
	return cubemap;
}

void Skybox::setEmission(float _emission)
{
	emission = _emission;
}

float Skybox::getEmission() const
{
	return emission;
}

void Skybox::create()
{
	Console::out::info("Skybox", "Generating skybox");

	// Set shader
	shader = ShaderPool::get("skybox");

	// Create vertices
	float vertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f 
	};

	// Create buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skybox::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	// Only render skybox if both cubemap and shader are available
	if (!cubemap || !shader) return;

	// Set depth function
	glDepthFunc(GL_LEQUAL);

	// Calculate skybox transformation matrices
	glm::mat4 adjustedViewMatrix = glm::mat4(glm::mat3(viewMatrix));
	glm::mat4 viewProjectionMatrix = projectionMatrix * adjustedViewMatrix;

	// Set skyvbox shader transformation matrices
	shader->bind();
	shader->setMatrix4("viewProjectionMatrix", viewProjectionMatrix);
	shader->setInt("skybox", 0);
	shader->setFloat("emission", emission);

	// Bind skybox vao
	glBindVertexArray(vao);

	// Bind cubemap texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->backendId());

	// Draw skybox
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Reset depth function
	glDepthFunc(GL_LESS);
}