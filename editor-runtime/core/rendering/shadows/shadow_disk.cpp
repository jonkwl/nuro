#include "shadow_disk.h"

#include <vector>
#include <random>
#include <glad/glad.h>

#include "../core/utils/console.h"

#define M_PI 3.14159265358979323846

ShadowDisk::ShadowDisk(uint32_t windowSize, uint32_t filterSize, uint32_t radius) : texture(0),
windowSize(windowSize),
filterSize(filterSize),
radius(radius)
{
	// Create data buffer
	std::vector<float> data;

	// Generate sample disk texture data with random samples
	int32_t bufferSize = windowSize * windowSize * filterSize * filterSize * 2;
	data.resize(bufferSize);
	size_t index = 0;
	for (int32_t texY = 0; texY < windowSize; texY++)
	{
		for (int32_t texX = 0; texX < windowSize; texX++)
		{
			for (int32_t v = filterSize - 1; v >= 0; v--)
			{
				for (int32_t u = 0; u < filterSize; u++)
				{
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
	int32_t nFilterSamples = filterSize * filterSize;

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

void ShadowDisk::bind(uint32_t unit)
{
	// Bind shadow disk texture to given unit
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, texture);
}

uint32_t ShadowDisk::getWindowSize()
{
	return windowSize;
}

uint32_t ShadowDisk::getFilterSize()
{
	return filterSize;
}

uint32_t ShadowDisk::getRadius()
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
