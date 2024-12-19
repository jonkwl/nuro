#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"

class Shader;

class BloomPass
{
public:
	explicit BloomPass(const Viewport& viewport);

	void create(const unsigned int mipDepth);
	void destroy();

	unsigned int render(const unsigned int hdrInput);

	float threshold;
	float softThreshold;
	float filterRadius;

private:
	unsigned int prefilteringPass(const unsigned int hdrInput);
	void downsamplingPass(const unsigned int hdrInput);
	void upsamplingPass();

private:
	const Viewport& viewport;

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