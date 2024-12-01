#pragma once

#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/postprocessing/motion_blur_pass.h"
#include "../src/rendering/postprocessing/bloom_pass.h"
#include "../src/viewport/viewport.h"

class Shader;

class PostProcessingPipeline
{
public:
	PostProcessingPipeline(const Viewport& viewport, const bool renderToScreen);

	void create();	// Create post processing pipeline
	void destroy(); // Destroy post processing pipeline

	void render(const unsigned int hdrInput, const unsigned int depthInput); // Render post processing by performing all post processing passes on input

	unsigned int getOutput(); // Get output of last post processing render

private:
	enum TextureUnits
	{
		HDR_UNIT,
		DEPTH_UNIT,
		BLOOM_UNIT,
		LENS_DIRT_UNIT
	};

	const Viewport& viewport;

	const bool renderToScreen;

	void syncConfiguration(); // Sync the post processing configuration with final pass shader

	unsigned int fbo;	 // Framebuffer
	unsigned int output; // Post processing output

	Shader* finalPassShader; // Post processing final pass shader

private:
	MotionBlurPass motionBlurPass;
	BloomPass bloomPass;
};