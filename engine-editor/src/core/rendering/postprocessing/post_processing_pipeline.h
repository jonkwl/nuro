#pragma once

#include <cstdint>

#include <rendering/postprocessing/post_processing.h>
#include <rendering/texture/texture.h>
#include <rendering/postprocessing/motion_blur_pass.h>
#include <rendering/postprocessing/bloom_pass.h>
#include <viewport/viewport.h>

class Shader;

class PostProcessingPipeline
{
public:
	explicit PostProcessingPipeline(const Viewport& viewport, const bool renderToScreen);

	void create();	// Create post processing pipeline
	void destroy(); // Destroy post processing pipeline

	void render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, const PostProcessing::Profile& profile, const uint32_t hdrInput, const uint32_t depthInput, const uint32_t velocityBufferInput); // Render post processing by performing all post processing passes on input

	uint32_t getOutput(); // Get output of last post processing render

private:
	static constexpr int32_t DEFAULT_BLOOM_MIP_DEPTH = 16;

	enum TextureUnits
	{
		HDR_UNIT,
		DEPTH_UNIT,
		BLOOM_UNIT,
		LENS_DIRT_UNIT
	};

	const Viewport& viewport;

	const bool renderToScreen;

	void syncConfiguration(const PostProcessing::Profile& profile); // Sync the post processing configuration with final pass shader

	uint32_t fbo;	 // Framebuffer
	uint32_t output; // Post processing output

	Shader* finalPassShader; // Post processing final pass shader

private:
	MotionBlurPass motionBlurPass;
	BloomPass bloomPass;
};