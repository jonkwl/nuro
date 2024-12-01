#pragma once

#include <vector>

#include "../src/viewport/viewport.h"
#include "../src/rendering/shader/shader.h"

class Entity;

class PrePass
{
public:
	PrePass(const Viewport& viewport);
	
	void create();
	void destroy();

	void render(std::vector<Entity*>& targets);

	unsigned int getDepthOutput();
	unsigned int getNormalOutput();

private:
	const Viewport& viewport;

	unsigned int fbo;
	unsigned int depthOutput;
	unsigned int normalOutput;

	Shader* prePassShader;
};