#include "input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/runtime/runtime.h"

namespace Input {

	GLFWwindow* _window;

	glm::vec2 _keyAxis = glm::vec2(0.0f);

	glm::vec2 _mouseAxis = glm::vec2(0.0f);
	glm::vec2 _mouseLast = glm::vec2(0.0f);

	glm::vec2 _scrollOffset = glm::vec2(0.0f);

	// Context callbacks

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		_scrollOffset = glm::vec2(xoffset, yoffset);
	}

	// Input functions

	void setupInputs()
	{
		_window = Runtime::glfw;

		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		_mouseLast = glm::vec2(mouseX, mouseY);

		glfwSetScrollCallback(_window, scroll_callback);
	}

	void updateInputs()
	{
		// set mouse
		double mouseX, mouseY;
		glfwGetCursorPos(Runtime::glfw, &mouseX, &mouseY);
		_mouseAxis = glm::vec2(mouseX - _mouseLast.x, -(mouseY - _mouseLast.y));
		_mouseLast = glm::vec2(mouseX, mouseY);

		if (glfwGetKey(Runtime::glfw, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(Runtime::glfw, true);
		}

		if (glfwGetKey(Runtime::glfw, GLFW_KEY_W) == GLFW_PRESS)
		{
			_keyAxis.x = 1.0f;
		}
		else if (glfwGetKey(Runtime::glfw, GLFW_KEY_S) == GLFW_PRESS)
		{
			_keyAxis.x = -1.0f;
		}
		else
		{
			_keyAxis.x = 0.0f;
		}

		if (glfwGetKey(Runtime::glfw, GLFW_KEY_D) == GLFW_PRESS)
		{
			_keyAxis.y = 1.0f;
		}
		else if (glfwGetKey(Runtime::glfw, GLFW_KEY_A) == GLFW_PRESS)
		{
			_keyAxis.y = -1.0f;
		}
		else
		{
			_keyAxis.y = 0.0f;
		}
	}

	// Getters

	const glm::vec2& getKeyAxis()
	{
		return _keyAxis;
	}

	const glm::vec2& getMouseAxis()
	{
		return _mouseAxis;
	}

	const glm::vec2& getScrollAxis()
	{
		glm::vec2 scrollAxis = _scrollOffset;
		_scrollOffset = glm::vec2(0.0f);
		return scrollAxis;
	}

}