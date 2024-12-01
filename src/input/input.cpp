#include "input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/runtime/runtime.h"

namespace Input {

	glm::vec2 _keyAxis = glm::vec2(0.0f);
	glm::vec2 _mouseAxis = glm::vec2(0.0f);
	glm::vec2 _mouseLast = glm::vec2(0.0f);

	void Input::setupInputs()
	{
		double mouseX, mouseY;
		glfwGetCursorPos(Runtime::glfw, &mouseX, &mouseY);
		_mouseLast = glm::vec2(mouseX, mouseY);
	}

	void Input::updateInputs()
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

	const glm::vec2& Input::getKeyAxis()
	{
		return _keyAxis;
	}

	const glm::vec2& Input::getMouseAxis()
	{
		return _mouseAxis;
	}

}