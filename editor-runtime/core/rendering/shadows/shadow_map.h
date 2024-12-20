#pragma once

#include <vector>
#include "../core/rendering/shader/shader.h"

class OldEntity;

class ShadowMap
{
public:
	explicit ShadowMap(unsigned int resolutionWidth, unsigned int resolutionHeight, float boundsWidth, float boundsHeight, float near, float far);

	void render();
	void bind(unsigned int unit);

	unsigned int getResolutionWidth() const;
	unsigned int getResolutionHeight() const;

	float getBoundsWidth() const;
	float getBoundsHeight() const;

	unsigned int getFramebuffer() const;

private:
	float near;
	float far;

	unsigned int resolutionWidth;
	unsigned int resolutionHeight;

	float boundsWidth;
	float boundsHeight;

	unsigned int texture;
	unsigned int framebuffer;

	Shader* shadowPassShader;
};