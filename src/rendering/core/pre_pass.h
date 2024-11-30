#pragma once

class PrePass
{
public:
	PrePass();
	
	void create();
	void destroy();

	void render();

	unsigned int getDepthOutput();
	unsigned int getNormalOutput();

	unsigned int getWidth();
	unsigned int getHeight();

private:
	unsigned int fbo;

	unsigned int depthOutput;
	unsigned int normalOutput;

	unsigned int width;
	unsigned int height;
};