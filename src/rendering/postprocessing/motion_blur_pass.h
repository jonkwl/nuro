#pragma once

#include <glm.hpp>

#include "../src/viewport/viewport.h"
#include "../src/rendering/postprocessing/velocity_buffer.h"

class Shader;

class MotionBlurPass
{
public:
	MotionBlurPass(const Viewport& viewport);

	void create();
	void destroy();

	unsigned int render(const unsigned int hdrInput, const unsigned int depthInput);

private:
	enum TextureUnits
	{
		HDR_UNIT,
		DEPTH_UNIT,
		VELOCITY_UNIT
	};

	const Viewport& viewport;

	unsigned int fbo;
	unsigned int output;

	Shader* shader;

	glm::mat4 previousViewProjectionMatrix;

	VelocityBuffer velocityBuffer;
};