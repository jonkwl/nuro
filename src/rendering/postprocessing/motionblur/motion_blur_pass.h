#pragma once

#include <glm.hpp>
#include <glad/glad.h>
#include "../src/rendering/shader/Shader.h"

class MotionBlurPass
{
public:
	static void setup();
	static unsigned int render(unsigned int hdrInput, unsigned int depthInput);
private:
	static unsigned int fbo;
	static unsigned int output;

	static Shader* shader;

	static glm::mat4 previousViewProjectionMatrix;
};