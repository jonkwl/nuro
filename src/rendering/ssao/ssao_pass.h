#pragma once

#include <glm.hpp>
#include <vector>

class Shader;

class SSAOPass
{
public:
	static void setup();														   // Setup ambient occlusion pass
	static unsigned int render(unsigned int depthInput, unsigned int normalInput); // Render ambient occlusion pass and return blurred output
private:
	static constexpr float AO_SCALE = 0.5f;			// Scale of ambient occlusion output
	static constexpr int MAX_KERNEL_SAMPLES = 64;	// Kernel samples initialized
	static constexpr float NOISE_RESOLUTION = 4.0f; // Size of noise texture

	enum TextureUnits
	{
		DEPTH_UNIT,
		NORMAL_UNIT,
		NOISE_UNIT,
		AO_UNIT
	};

	static unsigned int fbo;		   // Framebuffer
	static unsigned int aoOutput;	   // Ambient occlusion output
	static unsigned int blurredOutput; // Blurred ambient occlusion output

	static void ambientOcclusionPass(unsigned int depthInput, unsigned int normalInput);
	static void blurPass();

	static Shader* aoPassShader; // Ambient occlusion pass shader
	static Shader* aoBlurShader; // Composite shader

	static std::vector<glm::vec3> kernel; // Sample kernel
	static unsigned int noiseTexture;	  // Noise texture

	static std::vector<glm::vec3> generateKernel(); // Generate sample kernel
	static unsigned int generateNoiseTexture();		// Generate noise texture

	static float random();									// Get random
	static float lerp(float start, float end, float value); // Linear interpolation
};