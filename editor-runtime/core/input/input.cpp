#include "input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/context/application_context.h"

namespace Input {

	GLFWwindow* gWindow = nullptr;

	glm::vec2 gMoveAxis = glm::vec2(0.0f);

	glm::vec2 gMouseDelta = glm::vec2(0.0f);
	glm::vec2 gMouseLast = glm::vec2(0.0f);

	glm::vec2 gScrollOffset = glm::vec2(0.0f);

	// Context callbacks

	void _scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		gScrollOffset = glm::vec2(xoffset, yoffset);
	}

	// Input functions

	void setup()
	{
		gWindow = ApplicationContext::getWindow();

		double mouseX, mouseY;
		glfwGetCursorPos(gWindow, &mouseX, &mouseY);
		gMouseLast = glm::vec2(mouseX, mouseY);

		glfwSetScrollCallback(gWindow, _scrollCallback);
	}

	void update()
	{
		// set mouse
		double mouseX, mouseY;
		glfwGetCursorPos(gWindow, &mouseX, &mouseY);
		gMouseDelta = glm::vec2(mouseX - gMouseLast.x, -(mouseY - gMouseLast.y));
		gMouseLast = glm::vec2(mouseX, mouseY);

		if (glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(gWindow, true);
		}

		if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		{
			gMoveAxis.x = 1.0f;
		}
		else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		{
			gMoveAxis.x = -1.0f;
		}
		else
		{
			gMoveAxis.x = 0.0f;
		}

		if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		{
			gMoveAxis.y = 1.0f;
		}
		else if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		{
			gMoveAxis.y = -1.0f;
		}
		else
		{
			gMoveAxis.y = 0.0f;
		}
	}

	// Getters

	glm::vec2 moveAxis()
	{
		return gMoveAxis;
	}

	glm::vec2 mouseDelta()
	{
		return gMouseDelta;
	}

	glm::vec2 scrollDelta()
	{
		glm::vec2 scrollDelta = gScrollOffset;
		gScrollOffset = glm::vec2(0.0f);
		return scrollDelta;
	}

	glm::vec2 mousePosition()
	{
		double mouseX = 0.0, mouseY = 0.0;
		glfwGetCursorPos(gWindow, &mouseX, &mouseY);
		return glm::vec2(mouseX, mouseY);
	}

	bool keyDown(int32_t key)
	{
		return glfwGetKey(gWindow, key) == GLFW_PRESS;
	}

	bool mouseDown(int32_t mouseButton)
	{
		return glfwGetMouseButton(gWindow, mouseButton) == GLFW_PRESS;
	}

	bool command(int32_t key)
	{
		return keyDown(Key::LEFT_CONTROL) && keyDown(key);
	}

}