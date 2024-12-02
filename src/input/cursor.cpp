#include "cursor.h"

namespace Cursor {

	GLFWwindow* _window;

	void setContext(GLFWwindow* window)
	{
		_window = window;
	}

	glm::vec2 getPosition()
	{
		double mouseX = 0.0, mouseY = 0.0;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		return glm::vec2(mouseX, mouseY);
	}

	void setPosition(glm::vec2 position)
	{
		glfwSetCursorPos(_window, position.x, position.y);
	}

	void setType(int cursorType)
	{
		GLFWcursor* cursor = glfwCreateStandardCursor(cursorType);
		glfwSetCursor(_window, cursor);
		glfwDestroyCursor(cursor);
	}

	void setMode(int cursorMode)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, cursorMode);
	}

	void center()
	{
		int windowWidth, windowHeight;
		glfwGetWindowSize(_window, &windowWidth, &windowHeight);
		glm::vec2 cursorPosition = glm::vec2(windowWidth / 2.0f, windowHeight / 2.0f);
		glfwSetCursorPos(_window, cursorPosition.x, cursorPosition.y);
	}

}
