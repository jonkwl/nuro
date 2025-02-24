#include "cursor.h"

#include <utils/console.h>
#include <context/application_context.h>

namespace Cursor {

	GLFWwindow* gWindow;

	void setup()
	{
		gWindow = ApplicationContext::getWindow();
	}

	glm::vec2 getPosition()
	{
		double mouseX = 0.0, mouseY = 0.0;
		glfwGetCursorPos(gWindow, &mouseX, &mouseY);
		return glm::vec2(mouseX, mouseY);
	}

	glm::vec2 getScreenPosition()
	{
		// Get cursor position on window
		double mouseX, mouseY;
		glfwGetCursorPos(gWindow, &mouseX, &mouseY);

		// Get windows position on screen
		int windowPosX, windowPosY;
		glfwGetWindowPos(gWindow, &windowPosX, &windowPosY);

		// Calculate cursor position relative to the screen
		double screenX = windowPosX + mouseX;
		double screenY = windowPosY + mouseY;

		return glm::vec2(screenX, screenY);
	}

	void setPosition(glm::vec2 position)
	{
		glfwSetCursorPos(gWindow, position.x, position.y);
	}

	void setType(uint32_t cursorType)
	{
		glfwSetCursor(gWindow, glfwCreateStandardCursor(cursorType));
	}

	void setMode(uint32_t cursorMode)
	{
		glfwSetInputMode(gWindow, GLFW_CURSOR, cursorMode);
	}

	void center()
	{
		int32_t windowWidth, windowHeight;
		glfwGetWindowSize(gWindow, &windowWidth, &windowHeight);
		glm::vec2 cursorPosition = glm::vec2(windowWidth / 2.0f, windowHeight / 2.0f);
		glfwSetCursorPos(gWindow, cursorPosition.x, cursorPosition.y);
	}

}
