#pragma once

#include "../src/viewport/viewport.h"

class PrePass
{
public:
	PrePass(Viewport& viewport);
	
	void create();
	void destroy();

	void render();

	unsigned int getDepthOutput();
	unsigned int getNormalOutput();

private:
	Viewport& viewport;

	unsigned int fbo;
	unsigned int depthOutput;
	unsigned int normalOutput;
};