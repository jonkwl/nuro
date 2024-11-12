#pragma once

#include "../engine/rendering/shader/Shader.h"
#include "../engine/rendering/postprocessing/bloom/bloom_frame.h"

class BloomPass
{
public:
	void setup();
	unsigned int render(unsigned int input, float filterRadius);
private:
	void downsamplingPass(unsigned int input);
	void upsamplingPass(float filterRadius);

	BloomFrame bloomFrame;

	glm::vec2 fViewportSize;
	glm::ivec2 iViewportSize;

	Shader* downsamplingShader;
	Shader* upsamplingShader;
};

