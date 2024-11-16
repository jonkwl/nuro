#include "shadow_disk.h"

#include <glad/glad.h>
#include <cassert>

#include "../src/utils/log.h"

#define M_PI 3.14159265358979323846

ShadowDisk::ShadowDisk(unsigned int windowSize, unsigned int filterSize, unsigned int radius)
{
	// Set shadow disk parameters
	this->texture = 0;
	this->windowSize = windowSize;
	this->filterSize = filterSize;
	this->radius = radius;

	// Create data buffer
	std::vector<float> data;

	// Generate sample disk texture data with random samples
	int bufferSize = windowSize * windowSize * filterSize * filterSize * 2;
	data.resize(bufferSize);
	size_t index = 0;
	for (int texY = 0; texY < windowSize; texY++) {
		for (int texX = 0; texX < windowSize; texX++) {
			for (int v = filterSize - 1; v >= 0; v--) {
				for (int u = 0; u < filterSize; u++) {
					float x = ((float)u + 0.5f + random()) / (float)filterSize;
					float y = ((float)v + 0.5f + random()) / (float)filterSize;

					data[index] = sqrtf(y) * cosf(2 * M_PI * x);
					data[index + 1] = sqrtf(y) * sinf(2 * M_PI * x);
					index += 2;
				}
			}
		}
	}

	// Generate 3D sample disk texture
	int nFilterSamples = filterSize * filterSize;
	
	// Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, nFilterSamples / 2, windowSize, windowSize);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nFilterSamples / 2, windowSize, windowSize, GL_RGBA, GL_FLOAT, &data[0]);
	
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// Unbind texture
	glBindTexture(GL_TEXTURE_3D, 0);
}

void ShadowDisk::bind(unsigned int unit)
{
	// Bind shadow disk texture to given unit
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, texture);
}

unsigned int ShadowDisk::getWindowSize()
{
	return windowSize;
}

unsigned int ShadowDisk::getFilterSize()
{
	return filterSize;
}

unsigned int ShadowDisk::getRadius()
{
	return radius;
}

float ShadowDisk::random()
{
	static std::default_random_engine generator;
	// static std::mt19937 generator(std::random_device{}()); // Generator alternative
	static std::uniform_real_distribution<float> distribution(-0.5f, 0.5f);
	return distribution(generator);
}
