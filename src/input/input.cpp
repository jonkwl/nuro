#include "input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/runtime/runtime.h"

//
// TEMPORARY INPUT SYSTEM FOR BASIC INPUT HANDLING
//

glm::vec2 Input::keyAxis = glm::vec2(0.0f);
glm::vec2 Input::keyAxisSmooth = glm::vec2(0.0f);
float Input::keyAxisSmoothingFactor = 5.0f;

glm::vec2 Input::mouseAxis = glm::vec2(0.0f);
glm::vec2 Input::mouseLast = glm::vec2(0.0f);

void Input::setupInputs()
{
	double mouseX, mouseY;
	glfwGetCursorPos(Runtime::glfw, &mouseX, &mouseY);
	mouseLast = glm::vec2(mouseX, mouseY);
}

void Input::updateInputs()
{
	// set mouse
	double mouseX, mouseY;
	glfwGetCursorPos(Runtime::glfw, &mouseX, &mouseY);
	mouseAxis = glm::vec2(mouseX - mouseLast.x, -(mouseY - mouseLast.y));
	mouseLast = glm::vec2(mouseX, mouseY);

	// set axis inputs
	if (glfwGetKey(Runtime::glfw, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Runtime::glfw, true);
	}

	if (glfwGetKey(Runtime::glfw, GLFW_KEY_W) == GLFW_PRESS)
	{
		keyAxis.x = 1.0f;
	}
	else if (glfwGetKey(Runtime::glfw, GLFW_KEY_S) == GLFW_PRESS)
	{
		keyAxis.x = -1.0f;
	}
	else
	{
		keyAxis.x = 0.0f;
	}

	if (glfwGetKey(Runtime::glfw, GLFW_KEY_D) == GLFW_PRESS)
	{
		keyAxis.y = 1.0f;
	}
	else if (glfwGetKey(Runtime::glfw, GLFW_KEY_A) == GLFW_PRESS)
	{
		keyAxis.y = -1.0f;
	}
	else
	{
		keyAxis.y = 0.0f;
	}

	Input::keyAxisSmooth = glm::mix(Input::keyAxisSmooth, Input::keyAxis, Input::keyAxisSmoothingFactor * Runtime::deltaTime);
}