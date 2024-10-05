#pragma once

#include <glad/glad.h>
#include <glm.hpp>

#include "../engine/window/window.h"
#include "../engine/utils/log.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"

struct PostProcessingSetup {
	float exposure = 0.85f;
	float contrast = 1.0f;
	float gamma = 2.2f;

	bool chromaticAberration = true;
	float chromaticAberrationStrength = 0.5f;
	float chromaticAberrationRange = 0.2f;
	float chromaticAberrationRedOffset = 0.01f;
	float chromaticAberrationBlueOffset = 0.01f;

	bool vignette = true;
	float vignetteStrength = 1.0f;
	glm::vec3 vignetteColor = glm::vec3(0.0f, 0.0f, 0.0);
	float vignetteRadius = 0.7f;
	float vignetteSoftness = 0.35f;
	float vignetteRoundness = 1.35f;
};

class PostProcessing
{
public:
	static void initialize();
	static void bind();
	static void render();

	static PostProcessingSetup setup;
private:
	static unsigned int fbo;
	static unsigned int rbo;
	static unsigned int texture;
	static unsigned int vao;
	static unsigned int vbo;
	static Shader* shader;
};

