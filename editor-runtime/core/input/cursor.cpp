#include "cursor.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/context/application_context.h"

namespace Cursor {

	GLFWwindow* _window;

	void setup()
	{
		_window = ApplicationContext::getWindow();
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

	void setType(int32_t cursorType)
	{
		GLFWcursor* cursor = glfwCreateStandardCursor(cursorType);
		glfwSetCursor(_window, cursor);
		glfwDestroyCursor(cursor);
	}

	void setMode(int32_t cursorMode)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, cursorMode);
	}

	void center()
	{
		int32_t windowWidth, windowHeight;
		glfwGetWindowSize(_window, &windowWidth, &windowHeight);
		glm::vec2 cursorPosition = glm::vec2(windowWidth / 2.0f, windowHeight / 2.0f);
		glfwSetCursorPos(_window, cursorPosition.x, cursorPosition.y);
	}

}
