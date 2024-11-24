#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>
#include <random>

#include "../src/rendering/shader/Shader.h"

enum AmbientOcclusionUnits {
	AMBIENT_OCCLUSION_DEPTH_UNIT,
	AMBIENT_OCCLUSION_NORMAL_UNIT,
	AMBIENT_OCCLUSION_NOISE_UNIT,
	AMBIENT_OCCLUSION_HDR_UNIT,
	AMBIENT_OCCLUSION_AO_UNIT
};

class AmbientOcclusionPass
{
public:
	static void setup(); // Setup ambient occlusion pass
	static unsigned int render(unsigned int hdrInput, unsigned int depthInput, unsigned int normalInput); // Render ambient occlusion pass and return output
private:
	static unsigned int fbo; // Framebuffer

	static unsigned int aoOutput; // Ambient occlusion output
	static unsigned int compositeOutput; // Ambient occlusion applied on hdr input

	static void ambientOcclusionPass(unsigned int depthInput, unsigned int normalInput);
	static void compositePass(unsigned int hdrInput);

	static Shader* aoPassShader; // Ambient occlusion pass shader
	static Shader* compositeShader; // Composite shader

	static std::vector<glm::vec3> kernel; // Sample kernel
	static unsigned int noiseTexture; // Noise texture

	static std::vector<glm::vec3> generateKernel(unsigned int nSamples); // Generate sample kernel
	static unsigned int generateNoiseTexture(unsigned int nSamples, float size); // Generate noise texture

	static float random(); // Get random
	static float lerp(float start, float end, float value); // Linear interpolation
};