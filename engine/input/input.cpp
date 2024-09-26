#include "input.h"

glm::vec2 Input::keyAxis = glm::vec2(0.0f);
glm::vec2 Input::keyAxisSmooth = glm::vec2(0.0f);
float Input::keyAxisSmoothingFactor = 5.0f;

glm::vec2 Input::mouseAxis = glm::vec2(0.0f);
glm::vec2 Input::mouseLast = glm::vec2(0.0f);

bool Input::escPressed = false;

void Input::setupInputs() {

}

void Input::updateInputs() {
	// set mouse
	double mouseX, mouseY;
	glfwGetCursorPos(Context::window, &mouseX, &mouseY);
	Input::mouseAxis = glm::vec2(mouseX - Input::mouseLast.x, -(mouseY - Input::mouseLast.y));
	Input::mouseLast = glm::vec2(mouseX, mouseY);

	// set axis inputs
	if (glfwGetKey(Context::window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(Context::window, true);
	}

	if (glfwGetKey(Context::window, GLFW_KEY_W) == GLFW_PRESS) {
		keyAxis.x = 1.0f;
	}
	else if (glfwGetKey(Context::window, GLFW_KEY_S) == GLFW_PRESS) {
		keyAxis.x = -1.0f;
	}
	else {
		keyAxis.x = 0.0f;
	}

	if (glfwGetKey(Context::window, GLFW_KEY_D) == GLFW_PRESS) {
		keyAxis.y = 1.0f;
	}
	else if (glfwGetKey(Context::window, GLFW_KEY_A) == GLFW_PRESS) {
		keyAxis.y = -1.0f;
	}
	else {
		keyAxis.y = 0.0f;
	}

	Input::keyAxisSmooth = glm::mix(Input::keyAxisSmooth, Input::keyAxis, Input::keyAxisSmoothingFactor * Runtime::delta_time);

	// get escape input
	if (glfwGetKey(Context::window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (escPressed) return;
		escPressed = true;
		Runtime::showEngineUI = !Runtime::showEngineUI;
		if (Context::cursorMode == GLFW_CURSOR_NORMAL) {
			Context::setCursor(GLFW_CURSOR_DISABLED);
		}
		else {
			Context::setCursor(GLFW_CURSOR_NORMAL);
		}
	}
	else {
		escPressed = false;
	}
}