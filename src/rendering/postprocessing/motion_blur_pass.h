#pragma once

#include <glm.hpp>

#include "../src/rendering/postprocessing/velocity_buffer.h"

class Shader;

class MotionBlurPass
{
public:
	MotionBlurPass();

	void create();
	void destroy();

	unsigned int render(unsigned int hdrInput, unsigned int depthInput);

private:
	enum TextureUnits
	{
		HDR_UNIT,
		DEPTH_UNIT,
		VELOCITY_UNIT
	};

	unsigned int fbo;
	unsigned int output;

	Shader* shader;

	glm::mat4 previousViewProjectionMatrix;

	VelocityBuffer velocityBuffer;
};