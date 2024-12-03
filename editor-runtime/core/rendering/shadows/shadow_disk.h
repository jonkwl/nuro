#pragma once

class ShadowDisk
{
public:
	explicit ShadowDisk(unsigned int windowSize, unsigned int filterSize, unsigned int radius);
	void bind(unsigned int unit);

	unsigned int getWindowSize();
	unsigned int getFilterSize();
	unsigned int getRadius();

private:
	float random();

	unsigned int texture;

	unsigned int windowSize;
	unsigned int filterSize;
	unsigned int radius;
};
