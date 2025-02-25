#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include <viewport/viewport.h>
#include <rendering/shader/shader.h>

class PrePass
{
public:
	explicit PrePass(const Viewport& viewport);
	
	void create();
	void destroy();

	void render(glm::mat4 viewProjection, glm::mat3 viewNormal);

	uint32_t getDepthOutput();
	uint32_t getNormalOutput();

private:
	const Viewport& viewport;

	uint32_t fbo;
	uint32_t depthOutput;
	uint32_t normalOutput;

	Shader* prePassShader;
};