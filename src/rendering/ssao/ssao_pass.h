#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>
#include <random>

#include "../src/rendering/shader/Shader.h"

enum SSAOUnits {
	SSAO_DEPTH_UNIT,
	SSAO_NORMAL_UNIT,
	SSAO_NOISE_UNIT,
	SSAO_AO_UNIT
};

class SSAOPass
{
public:
	static void setup(); // Setup ambient occlusion pass
	static unsigned int render(unsigned int depthInput, unsigned int normalInput); // Render ambient occlusion pass and return output
private:
	static unsigned int fbo; // Framebuffer

	static unsigned int rawOutput; // Raw ambient occlusion output
	static unsigned int output; // Final blurred ambient occlusion output

	static void ambientOcclusionPass(unsigned int depthInput, unsigned int normalInput);
	static void blurPass();

	static Shader* aoPassShader; // Ambient occlusion pass shader
	static Shader* aoBlurShader; // Composite shader

	static glm::vec2 aoResolution; // Height and width of ambient occlusion output
	static const unsigned int nMaxSamples; // Fixed maximum kernel sample amount
	static const float noiseSize; // Fixed noise size

	static std::vector<glm::vec3> kernel; // Sample kernel
	static unsigned int noiseTexture; // Noise texture

	static std::vector<glm::vec3> generateKernel(unsigned int nSamples); // Generate sample kernel
	static unsigned int generateNoiseTexture(unsigned int nSamples, float size); // Generate noise texture

	static float random(); // Get random
	static float lerp(float start, float end, float value); // Linear interpolation
};