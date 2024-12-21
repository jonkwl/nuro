#include "skybox.h"

#include <glad/glad.h>

#include "../core/rendering/skybox/cubemap.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shader/shader.h"
#include "../core/utils/log.h"

Skybox::Skybox() :
	emission(0.0f),
	cubemapTexture(0),
	vao(0),
	vbo(0),
	shader(nullptr)
{
}

Skybox::Skybox(Cubemap& cubemap, Shader* customShader) :
	emission(1.0f),
	cubemapTexture(0),
	vao(0),
	vbo(0),
	shader(ShaderPool::get("skybox"))
{
	if (customShader) {
		shader = customShader;
	}

	generate(cubemap);
}

void Skybox::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	// Only render if skybox was created
	if (!vao) return;

	// DEPTH FUNCTION NEEDS TO BE GL_LEQUAL!

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
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// Draw skybox
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Skybox::generate(Cubemap& cubemap)
{
	Log::printProcessStart("Skybox", "Generating skybox from cubemap " + cubemap.name + "...");

	// Create 
	createTextures(cubemap);
	createBuffers();

	Log::printProcessDone("Skybox", "Skybox generated");
}

void Skybox::createTextures(Cubemap& cubemap)
{
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	Log::printProcessInfo("Loading textures...");
	for (int32_t i = 0; i < cubemap.faces.size(); i++)
	{
		CubemapFace face = cubemap.faces[i];

		GLenum format = GL_RGB;
		if (face.channels == 4)
		{
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, face.width, face.height, 0, format, GL_UNSIGNED_BYTE, face.data.data());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::createBuffers()
{
	Log::printProcessInfo("Generating sky...");

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
		1.0f, -1.0f, 1.0f };

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