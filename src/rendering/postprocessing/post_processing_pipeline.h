#pragma once

#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/postprocessing/motion_blur_pass.h"
#include "../src/rendering/postprocessing/bloom_pass.h"

class Shader;

class PostProcessingPipeline
{
public:
	static void setup();					   // Setup post processing
	static void render(unsigned int hdrInput); // Render post processing by performing all post processing passes on input

	static unsigned int getOutput(); // Get output of last post processing render

private:
	enum TextureUnits
	{
		HDR_UNIT,
		DEPTH_UNIT,
		BLOOM_UNIT,
		LENS_DIRT_UNIT
	};

	static void syncConfiguration(); // Sync the post processing configuration with final pass shader

	static Shader *finalPassShader; // Post processing final pass shader

	static unsigned int fbo;	// Framebuffer
	static unsigned int output; // Post processing output

	static PostProcessing::Configuration& configuration;

private:
	static MotionBlurPass motionBlurPass;
	static BloomPass bloomPass;
};