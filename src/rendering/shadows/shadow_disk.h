#pragma once

#include <vector>
#include <random>

class ShadowDisk
{
public:
	static void setup(unsigned int windowSize, unsigned int filterSize, unsigned int radius);
	static void bind(unsigned int unit);

	static unsigned int getWindowSize();
	static unsigned int getFilterSize();
	static unsigned int getRadius();
private:
	static float random();

	static unsigned int texture;

	static unsigned int windowSize;
	static unsigned int filterSize;
	static unsigned int radius;
};

