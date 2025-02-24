#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include <viewport/viewport.h>
#include <rendering/postprocessing/post_processing.h>

class Shader;

class SSAOPass
{
public:
	explicit SSAOPass(Viewport& viewport);

	void create(float aoScale = 0.5f, int32_t maxKernelSamples = 64, float noiseResolution = 4.0f);  // Create ambient occlusion pass
	void destroy(); // Destroy ambient occlusion pass

	uint32_t render(const glm::mat4& projection, const PostProcessing::Profile& profile, uint32_t depthInput, uint32_t normalInput); // Render ambient occlusion pass and return blurred output

	uint32_t getOutputRaw(); // Returns ao output (raw ssao texture)
	uint32_t getOutputProcessed(); // Returns blurred output (processed ssao texture)
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
	int32_t maxKernelSamples; // Amount of kernel samples being generated (therefore the max amount to be utilised)
	float noiseResolution; // Resolution of noise texture

	uint32_t fbo;		   // Framebuffer
	uint32_t aoOutput;	   // Ambient occlusion output
	uint32_t blurredOutput; // Blurred ambient occlusion output

	void ambientOcclusionPass(const glm::mat4& projection, const PostProcessing::Profile& profile, uint32_t depthInput, uint32_t normalInput);
	void blurPass(const PostProcessing::Profile& profile);

	Shader* aoPassShader; // Ambient occlusion pass shader
	Shader* aoBlurShader; // Composite shader

	std::vector<glm::vec3> kernel; // Sample kernel
	uint32_t noiseTexture;	  // Noise texture

	std::vector<glm::vec3> generateKernel(); // Generate sample kernel
	uint32_t generateNoiseTexture();		// Generate noise texture

	float random();									// Get random
	float lerp(float start, float end, float value); // Linear interpolation
};