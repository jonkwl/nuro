#pragma once

#include <glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Input
{

	void setup(GLFWwindow* linkedWindow); // Setup input system
	void step(); // Fetch inputs of current frame

	const glm::vec2& getKeyAxis();
	const glm::vec2& getMouseAxis();
	const glm::vec2& getScrollAxis();

};