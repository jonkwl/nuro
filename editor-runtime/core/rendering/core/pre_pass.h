#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/shader/shader.h"

class OldEntity;

class PrePass
{
public:
	explicit PrePass(const Viewport& viewport);
	
	void create();
	void destroy();

	void render(glm::mat4 viewProjection, glm::mat3 viewNormal);

	unsigned int getDepthOutput();
	unsigned int getNormalOutput();

private:
	const Viewport& viewport;

	unsigned int fbo;
	unsigned int depthOutput;
	unsigned int normalOutput;

	Shader* prePassShader;
};