#include "input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/context/application_context.h"

namespace Input {

	GLFWwindow* _window = nullptr;

	glm::vec2 _moveAxis = glm::vec2(0.0f);

	glm::vec2 _mouseDelta = glm::vec2(0.0f);
	glm::vec2 _mouseLast = glm::vec2(0.0f);

	glm::vec2 _scrollOffset = glm::vec2(0.0f);

	// Context callbacks

	void _scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		_scrollOffset = glm::vec2(xoffset, yoffset);
	}

	// Input functions

	void setup()
	{
		_window = ApplicationContext::getWindow();

		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		_mouseLast = glm::vec2(mouseX, mouseY);

		glfwSetScrollCallback(_window, _scrollCallback);
	}

	void update()
	{
		// set mouse
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		_mouseDelta = glm::vec2(mouseX - _mouseLast.x, -(mouseY - _mouseLast.y));
		_mouseLast = glm::vec2(mouseX, mouseY);

		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(_window, true);
		}

		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			_moveAxis.x = 1.0f;
		}
		else if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			_moveAxis.x = -1.0f;
		}
		else
		{
			_moveAxis.x = 0.0f;
		}

		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		{
			_moveAxis.y = 1.0f;
		}
		else if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		{
			_moveAxis.y = -1.0f;
		}
		else
		{
			_moveAxis.y = 0.0f;
		}
	}

	// Getters

	glm::vec2 moveAxis()
	{
		return _moveAxis;
	}

	glm::vec2 mouseDelta()
	{
		return _mouseDelta;
	}

	glm::vec2 scrollDelta()
	{
		glm::vec2 scrollDelta = _scrollOffset;
		_scrollOffset = glm::vec2(0.0f);
		return scrollDelta;
	}

	glm::vec2 mousePosition()
	{
		double mouseX = 0.0, mouseY = 0.0;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		return glm::vec2(mouseX, mouseY);
	}

	bool keyDown(int32_t key)
	{
		return glfwGetKey(_window, key) == GLFW_PRESS;
	}

	bool mouseDown(int32_t mouseButton)
	{
		return glfwGetMouseButton(_window, mouseButton) == GLFW_PRESS;
	}

	bool command(int32_t key)
	{
		return keyDown(Key::LEFT_CONTROL) && keyDown(key);
	}

}