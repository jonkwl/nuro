#pragma once

#include <glad/glad.h>
#include <glm.hpp>

#include "../engine/window/window.h"
#include "../engine/utils/log.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/primitives/quad.h"
#include "../engine/rendering/postprocessing/debug/debug_pass.h"
#include "../engine/rendering/postprocessing/bloom/bloom_pass.h"

enum FinalPassTextureSlots {
	HDR_BUFFER_UNIT,
	BLOOM_BUFFER_UNIT,
	LENS_DIRT_UNIT
};

struct PostProcessingConfiguration {
	float exposure = 1.0f;
	float contrast = 1.006f;
	float gamma = 2.2f;

	float bloomIntensity = 0.015f;
	glm::vec3 bloomColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float bloomThreshold = 0.25f;
	float bloomSoftThreshold = 0.0f;
	float bloomFilterRadius = 0.0f;

	bool lensDirt = false;
	Texture* lensDirtTexture = nullptr;
	float lensDirtIntensity = 0.0f;

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
	static void setup();
	static void render(unsigned int input);

	static PostProcessingConfiguration configuration;
private:
	static void syncConfiguration();

	static Shader* finalPassShader;
};