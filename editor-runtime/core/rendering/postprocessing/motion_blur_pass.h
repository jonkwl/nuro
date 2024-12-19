#pragma once

#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/postprocessing/post_processing.h"

class Shader;

class MotionBlurPass
{
public:
	explicit MotionBlurPass(const Viewport& viewport);

	void create();
	void destroy();

	unsigned int render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, const PostProcessing::Profile& profile, const unsigned int hdrInput, const unsigned int depthInput, const unsigned int velocityBufferInput);

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
};