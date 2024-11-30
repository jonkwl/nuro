#pragma once

#include <vector>
#include <glm.hpp>

#include "../src/viewport/viewport.h"

class Shader;

class BloomPass
{
public:
	BloomPass(Viewport& viewport);

	void create(unsigned int mipDepth);
	void destroy();

	unsigned int render(unsigned int hdrInput);

	float threshold;
	float softThreshold;
	float filterRadius;
	unsigned int mipDepth;

private:
	unsigned int prefilteringPass(unsigned int hdrInput);
	void downsamplingPass(unsigned int hdrInput);
	void upsamplingPass();

private:
	Viewport& viewport;

	struct Mip
	{
		glm::ivec2 iSize;
		glm::vec2 fSize;
		glm::vec2 inversedSize;
		unsigned int texture;
	};
	std::vector<Mip> mipChain;

	glm::ivec2 iViewportSize;
	glm::vec2 fViewportSize;
	glm::vec2 inversedViewportSize;

	unsigned int framebuffer;
	unsigned int prefilterOutput;

	Shader* prefilterShader;
	Shader* downsamplingShader;
	Shader* upsamplingShader;
};