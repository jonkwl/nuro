#include "viewport.h"

#include <algorithm>

Viewport::Viewport() : width(0.0f), height(0.0f)
{
	resize(width, height);
}

Viewport::Viewport(float _width, float _height) : width(_width), height(_height)
{
	resize(width, height);
}

void Viewport::resize(float _width, float _height)
{
	_width = std::max(_width, 1.0f);
	_height = std::max(_height, 1.0f);

	width = _width;
	height = _height;
}

float Viewport::getWidth() const
{
	return width;
}

float Viewport::getHeight() const
{
	return height;
}

uint32_t Viewport::getWidth_i() const
{
	return static_cast<uint32_t>(width);
}

uint32_t Viewport::getHeight_i() const
{
	return static_cast<uint32_t>(height);
}

GLsizei Viewport::getWidth_gl() const
{
	return static_cast<GLsizei>(width);
}

GLsizei Viewport::getHeight_gl() const
{
	return static_cast<GLsizei>(height);
}

glm::vec2 Viewport::getResolution() const
{
	return glm::vec2(width, height);
}

glm::ivec2 Viewport::getResolution_i() const
{
	return glm::ivec2(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

float Viewport::getAspect() const
{
	return width / height;
}
