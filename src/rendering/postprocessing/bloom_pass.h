#pragma once

#include <vector>
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

private:
	struct Mip
	{
		glm::ivec2 iSize;
		glm::vec2 fSize;
		glm::vec2 inversedSize;
		unsigned int texture;
	};
	static std::vector<Mip> mipChain;

	static glm::ivec2 iViewportSize;
	static glm::vec2 fViewportSize;
	static glm::vec2 inversedViewportSize;

	static unsigned int framebuffer;
	static unsigned int prefilterOutput;

	static Shader *prefilterShader;
	static Shader *downsamplingShader;
	static Shader *upsamplingShader;
};