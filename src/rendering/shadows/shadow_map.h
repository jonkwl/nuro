#pragma once

#include "../src/camera/camera.h"

class ShadowMap
{
public:
	explicit ShadowMap(unsigned int resolutionWidth, unsigned int resolutionHeight, float boundsWidth, float boundsHeight);

	void render(Camera& camera);
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