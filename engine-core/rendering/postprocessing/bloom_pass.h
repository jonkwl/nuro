#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include <viewport/viewport.h>

class Shader;

class BloomPass
{
public:
	explicit BloomPass(const Viewport& viewport);

	void create(const uint32_t mipDepth);
	void destroy();

	uint32_t render(const uint32_t hdrInput);

	float threshold;
	float softThreshold;
	float filterRadius;

private:
	uint32_t prefilteringPass(const uint32_t hdrInput);
	void downsamplingPass(const uint32_t hdrInput);
	void upsamplingPass();

private:
	const Viewport& viewport;

	struct Mip
	{
		glm::ivec2 iSize = glm::ivec2(0, 0);
		glm::vec2 fSize = glm::vec2(0.0f, 0.0f);
		glm::vec2 inversedSize = glm::vec2(0.0f, 0.0f);
		uint32_t texture = 0;
	};
	std::vector<Mip> mipChain;

	glm::ivec2 iViewportSize;
	glm::vec2 fViewportSize;
	glm::vec2 inversedViewportSize;

	uint32_t framebuffer;
	uint32_t prefilterOutput;

	Shader* prefilterShader;
	Shader* downsamplingShader;
	Shader* upsamplingShader;
};