#pragma once

#include <glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Input
{

	void setContext(GLFWwindow* window); // Sets context for input system
	void step(); // Fetch inputs of current frame

	glm::vec2 getKeyAxis();
	glm::vec2 getMouseAxis();
	glm::vec2 getScrollAxis();

	glm::vec2 getMousePosition();

};