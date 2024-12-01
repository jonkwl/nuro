#pragma once

#include "../src/viewport/viewport.h"

class PrePass
{
public:
	PrePass(const Viewport& viewport);
	
	void create();
	void destroy();

	void render();

	unsigned int getDepthOutput();
	unsigned int getNormalOutput();

private:
	const Viewport& viewport;

	unsigned int fbo;
	unsigned int depthOutput;
	unsigned int normalOutput;
};