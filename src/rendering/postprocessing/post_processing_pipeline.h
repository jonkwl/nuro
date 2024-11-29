#pragma once

#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/postprocessing/motion_blur_pass.h"
#include "../src/rendering/postprocessing/bloom_pass.h"

class Shader;

class PostProcessingPipeline
{
public:
	PostProcessingPipeline();

	void create();	// Create post processing pipeline
	void destroy(); // Destroy post processing pipeline

	void render(unsigned int hdrInput); // Render post processing by performing all post processing passes on input

	unsigned int getOutput(); // Get output of last post processing render

private:
	bool created;

	enum TextureUnits
	{
		HDR_UNIT,
		DEPTH_UNIT,
		BLOOM_UNIT,
		LENS_DIRT_UNIT
	};

	void syncConfiguration(); // Sync the post processing configuration with final pass shader

	unsigned int fbo;	 // Framebuffer
	unsigned int output; // Post processing output

	Shader *finalPassShader; // Post processing final pass shader

	PostProcessing::Configuration &configuration; // Configuration

private:
	MotionBlurPass motionBlurPass;
	BloomPass bloomPass;
};