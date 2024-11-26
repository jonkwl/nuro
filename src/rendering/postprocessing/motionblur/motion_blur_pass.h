#pragma once

#include <glm.hpp>

class Shader;

class MotionBlurPass
{
public:
	static void setup();
	static unsigned int render(unsigned int hdrInput, unsigned int depthInput);
private:
	enum TextureUnits {
		HDR_UNIT,
		DEPTH_UNIT,
		VELOCITY_UNIT
	};

	static unsigned int fbo;
	static unsigned int output;

	static Shader* shader;

	static glm::mat4 previousViewProjectionMatrix;
};