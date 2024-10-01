#include "input.h"

glm::vec2 Input::keyAxis = glm::vec2(0.0f);
glm::vec2 Input::keyAxisSmooth = glm::vec2(0.0f);
float Input::keyAxisSmoothingFactor = 5.0f;

glm::vec2 Input::mouseAxis = glm::vec2(0.0f);
glm::vec2 Input::mouseLast = glm::vec2(0.0f);

bool Input::escPressed = false;

void Input::setupInputs() {
	double mouseX, mouseY;
	glfwGetCursorPos(Window::glfw, &mouseX, &mouseY);
	mouseLast = glm::vec2(mouseX, mouseY);
}

void Input::updateInputs() {
	// set mouse
	double mouseX, mouseY;
	glfwGetCursorPos(Window::glfw, &mouseX, &mouseY);
	mouseAxis = glm::vec2(mouseX - mouseLast.x, -(mouseY - mouseLast.y));
	mouseLast = glm::vec2(mouseX, mouseY);

	// set axis inputs
	if (glfwGetKey(Window::glfw, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(Window::glfw, true);
	}

	if (glfwGetKey(Window::glfw, GLFW_KEY_W) == GLFW_PRESS) {
		keyAxis.x = 1.0f;
	}
	else if (glfwGetKey(Window::glfw, GLFW_KEY_S) == GLFW_PRESS) {
		keyAxis.x = -1.0f;
	}
	else {
		keyAxis.x = 0.0f;
	}

	if (glfwGetKey(Window::glfw, GLFW_KEY_D) == GLFW_PRESS) {
		keyAxis.y = 1.0f;
	}
	else if (glfwGetKey(Window::glfw, GLFW_KEY_A) == GLFW_PRESS) {
		keyAxis.y = -1.0f;
	}
	else {
		keyAxis.y = 0.0f;
	}

	Input::keyAxisSmooth = glm::mix(Input::keyAxisSmooth, Input::keyAxis, Input::keyAxisSmoothingFactor * Runtime::deltaTime);

	// get escape input
	if (glfwGetKey(Window::glfw, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (escPressed) return;
		escPressed = true;
		Runtime::showEngineUI = !Runtime::showEngineUI;
		if (Window::cursorMode == GLFW_CURSOR_NORMAL) {
			Window::setCursor(GLFW_CURSOR_DISABLED);
		}
		else {
			Window::setCursor(GLFW_CURSOR_NORMAL);
		}
	}
	else {
		escPressed = false;
	}
}