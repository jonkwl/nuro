#pragma once

#include <cstdint>

class ShadowDisk
{
public:
	explicit ShadowDisk(uint32_t windowSize, uint32_t filterSize, uint32_t radius);
	void bind(uint32_t unit);

	uint32_t getWindowSize();
	uint32_t getFilterSize();
	uint32_t getRadius();

private:
	float random();

	uint32_t texture;

	uint32_t windowSize;
	uint32_t filterSize;
	uint32_t radius;
};
