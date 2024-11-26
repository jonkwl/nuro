#pragma once

#include <glm.hpp>
#include <vector> 

struct BloomMip {
	glm::ivec2 iSize;
	glm::vec2 fSize;
	glm::vec2 inversedSize;
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
	static unsigned int framebuffer;
	static unsigned int prefilterTexture;
	static std::vector<BloomMip> mipChain;
};

