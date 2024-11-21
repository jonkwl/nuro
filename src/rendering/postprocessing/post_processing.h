#pragma once

#include <glad/glad.h>
#include <glm.hpp>

#include "../src/window/window.h"
#include "../src/utils/log.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/rendering/postprocessing/debug/debug_pass.h"
#include "../src/rendering/postprocessing/bloom/bloom_pass.h"
#include "../src/rendering/postprocessing/motionblur/motion_blur_pass.h"

enum FinalPassTextureSlots {
	HDR_BUFFER_UNIT,
	DEPTH_BUFFER,
	BLOOM_BUFFER_UNIT,
	LENS_DIRT_UNIT
};

struct PostProcessingConfiguration {
	bool colorGrading = false;
	float exposure = 1.0f;
	float contrast = 1.004f;
	float gamma = 2.2f;

	bool motionBlur = true;
	float motionBlurIntensity = 1.0f;
	int motionBlurSamples = 64;

	bool bloom = true;
	float bloomIntensity = 0.35f;
	float bloomColor[3] = { 1.0f, 1.0f, 1.0f };
	float bloomThreshold = 0.2f;
	float bloomSoftThreshold = 0.0f;
	float bloomFilterRadius = 0.0f;
	unsigned int bloomMipDepth = 16;
	bool lensDirt = false;
	Texture* lensDirtTexture = nullptr;
	float lensDirtIntensity = 0.0f;

	bool chromaticAberration = true;
	float chromaticAberrationIntensity = 0.5f;
	float chromaticAberrationRange = 0.2f;
	float chromaticAberrationRedOffset = 0.01f;
	float chromaticAberrationBlueOffset = 0.01f;

	bool vignette = true;
	float vignetteIntensity = 1.0f;
	float vignetteColor[3] = { 0.0f, 0.0f, 0.0f };
	float vignetteRadius = 0.68f;
	float vignetteSoftness = 0.35f;
	float vignetteRoundness = 1.8f;

	bool ambientOcclusion = false;
};

class PostProcessing
{
public:
	static void setup();
	static void render(unsigned int hdrInput);

	static PostProcessingConfiguration configuration;

	static unsigned int getOutput();
private:
	static void syncConfiguration();

	static Shader* finalPassShader;

	static PostProcessingConfiguration defaultConfiguration;

	static unsigned int framebuffer;
	static unsigned int output;
};