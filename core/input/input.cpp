#include "input.h"

namespace Input {

	GLFWwindow* _window = nullptr;

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

	void setContext(GLFWwindow* window)
	{
		_window = window;

		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		_mouseLast = glm::vec2(mouseX, mouseY);

		glfwSetScrollCallback(_window, scroll_callback);
	}

	void step()
	{
		// set mouse
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		_mouseAxis = glm::vec2(mouseX - _mouseLast.x, -(mouseY - _mouseLast.y));
		_mouseLast = glm::vec2(mouseX, mouseY);

		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(_window, true);
		}

		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			_keyAxis.x = 1.0f;
		}
		else if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			_keyAxis.x = -1.0f;
		}
		else
		{
			_keyAxis.x = 0.0f;
		}

		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		{
			_keyAxis.y = 1.0f;
		}
		else if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		{
			_keyAxis.y = -1.0f;
		}
		else
		{
			_keyAxis.y = 0.0f;
		}
	}

	// Getters

	glm::vec2 getKeyAxis()
	{
		return _keyAxis;
	}

	glm::vec2 getMouseAxis()
	{
		return _mouseAxis;
	}

	glm::vec2 getScrollAxis()
	{
		glm::vec2 scrollAxis = _scrollOffset;
		_scrollOffset = glm::vec2(0.0f);
		return scrollAxis;
	}

	glm::vec2 getMousePosition()
	{
		double mouseX = 0.0, mouseY = 0.0;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		return glm::vec2(mouseX, mouseY);
	}

}