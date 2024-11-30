#pragma once

#include <glm.hpp>
#include <vector>

#include "../src/viewport/viewport.h"

class Shader;

class SSAOPass
{
public:
	SSAOPass(Viewport& viewport);

	void create(float aoScale = 0.5f, int maxKernelSamples = 64, float noiseResolution = 4.0f);  // Create ambient occlusion pass
	void destroy(); // Destroy ambient occlusion pass

	unsigned int render(unsigned int depthInput, unsigned int normalInput); // Render ambient occlusion pass and return blurred output
private:
	enum TextureUnits
	{
		DEPTH_UNIT,
		NORMAL_UNIT,
		NOISE_UNIT,
		AO_UNIT
	};

	Viewport& viewport;

	float aoScale; // Scale of ambient occlusion resolution in relation to viewport resolution
	int maxKernelSamples; // Amount of kernel samples being generated (therefore the max amount to be utilised)
	float noiseResolution; // Resolution of noise texture

	unsigned int fbo;		   // Framebuffer
	unsigned int aoOutput;	   // Ambient occlusion output
	unsigned int blurredOutput; // Blurred ambient occlusion output

	void ambientOcclusionPass(unsigned int depthInput, unsigned int normalInput);
	void blurPass();

	Shader* aoPassShader; // Ambient occlusion pass shader
	Shader* aoBlurShader; // Composite shader

	std::vector<glm::vec3> kernel; // Sample kernel
	unsigned int noiseTexture;	  // Noise texture

	std::vector<glm::vec3> generateKernel(); // Generate sample kernel
	unsigned int generateNoiseTexture();		// Generate noise texture

	float random();									// Get random
	float lerp(float start, float end, float value); // Linear interpolation
};