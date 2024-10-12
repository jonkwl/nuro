#pragma once

#include <glad/glad.h>
#include <glm.hpp>

#include "../engine/window/window.h"
#include "../engine/utils/log.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"

struct PostProcessingSetup {
	unsigned int msaaSamples = 4;

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
	float vignetteRadius = 0.68f;
	float vignetteSoftness = 0.35f;
	float vignetteRoundness = 1.8f;
};

class PostProcessing
{
public:
	static void initialize();
	static void bind();
	static void render();

	static PostProcessingSetup setup;
private:
	static unsigned int screenTexture; // Texture to be rendered on screen

	static unsigned int ppFbo; // Post processing framebuffer
	static unsigned int ppRbo; // Post processing renderbuffer

	static unsigned int msaaFbo; // Anti-Aliasing framebuffer
	static unsigned int msaaRbo; // Anti-Aliasing renderbuffer
	static unsigned int msaaColorBuffer; // Anti-Aliasing color buffer texture

	static unsigned int renderVao; // Rendered quad vertex array object
	static unsigned int renderVbo; // Rendered quad vertex buffer object
	static Shader* shader;
};

