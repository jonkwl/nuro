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

private:
	unsigned int fbo;
	unsigned int depthOutput;
	unsigned int normalOutput;
};