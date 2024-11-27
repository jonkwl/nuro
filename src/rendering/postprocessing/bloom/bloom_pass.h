#pragma once

#include <glm.hpp>

class Shader;

class BloomPass
{
public:
	static void setup();
	static unsigned int render(unsigned int hdrInput);

	static float threshold;
	static float softThreshold;
	static float filterRadius;
	static unsigned int mipDepth;

private:
	static unsigned int prefilteringPass(unsigned int hdrInput);
	static void downsamplingPass(unsigned int hdrInput);
	static void upsamplingPass();

	static glm::ivec2 iViewportSize;
	static glm::vec2 fViewportSize;
	static glm::vec2 inversedViewportSize;

	static Shader *prefilterShader;
	static Shader *downsamplingShader;
	static Shader *upsamplingShader;
};