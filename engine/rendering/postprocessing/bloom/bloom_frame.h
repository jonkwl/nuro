#pragma once

#include <vector> 
#include <glm.hpp>
#include <glad/glad.h>

struct BloomMip {
	glm::vec2 fSize;
	glm::ivec2 iSize;
	unsigned int texture;
};

class BloomFrame
{
public:
	void setup(unsigned int mipDepth);
	void bind();

	const unsigned int getPrefilterTexture() const;
	const std::vector<BloomMip>& getMipChain() const;
private:
	unsigned int fbo;
	unsigned int prefilterTexture;
	std::vector<BloomMip> mipChain;
};

