#pragma once

class PrePass
{
public:
	static void setup(unsigned int width, unsigned int height);

	static void render();

	static unsigned int getDepthOutput();
	static unsigned int getNormalOutput();

	static unsigned int getWidth();
	static unsigned int getHeight();

private:
	static unsigned int fbo;

	static unsigned int depthOutput;
	static unsigned int normalOutput;

	static unsigned int width;
	static unsigned int height;
};