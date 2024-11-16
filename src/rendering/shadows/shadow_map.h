#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

#include "../src/rendering/shader/Shader.h"

class ShadowMap
{
public:
	ShadowMap(unsigned int resolutionWidth, unsigned int resolutionHeight, float boundsWidth, float boundsHeight);

	void render();
	void bind(unsigned int unit);

	unsigned int getResolutionWidth() const;
	unsigned int getResolutionHeight() const;

	float getBoundsWidth() const;
	float getBoundsHeight() const;

	unsigned int getFramebuffer() const;
private:
	unsigned int resolutionWidth;
	unsigned int resolutionHeight;

	float boundsWidth;
	float boundsHeight;

	unsigned int texture;
	unsigned int framebuffer;
};