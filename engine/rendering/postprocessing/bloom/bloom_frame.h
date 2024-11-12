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
	static void setup(unsigned int mipDepth);
	static void bind();

	static unsigned int getPrefilterTexture();
	static std::vector<BloomMip>& getMipChain();
private:
	static unsigned int fbo;
	static unsigned int prefilterTexture;
	static std::vector<BloomMip> mipChain;
};

