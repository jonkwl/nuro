#pragma once

#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Viewport
{
public:
	Viewport();
	Viewport(float width, float height);

	// Resizes the viewport to the specified width and height
	void resize(float width, float height);

	// Returns the width of the viewport
	float getWidth() const;

	// Returns the height of the viewport
	float getHeight() const;

	// Returns the width of the viewport as an unsigned integer
	uint32_t getWidth_i() const;

	// Returns the height of the viewport as an unsigned integer
	uint32_t getHeight_i() const;

	// Returns the width of the viewport as a gl size type
	GLsizei getWidth_gl() const;

	// Returns the height of the viewport as a gl size type
	GLsizei getHeight_gl() const;

	// Returns the resolution of the viewport as a vector
	glm::vec2 getResolution() const;

	// Returns the resolution of the viewport as an integer vector
	glm::ivec2 getResolution_i() const;

	// Returns the aspect ratio of the viewport
	float getAspect() const;

private:
	float width;
	float height;
};