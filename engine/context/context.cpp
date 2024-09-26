#include "context.h"

GLFWwindow* Context::window = nullptr;

int Context::viewport_x = 0;
int Context::viewport_y = 0;

int Context::width = 800;
int Context::height = 600;

std::string Context::title = "Rendering Alpha";

bool Context::fullscreen = true;

GLenum Context::cursorMode = GLFW_CURSOR_DISABLED;

void Context::setCursor(GLenum cusor_mode) {
	cursorMode = cusor_mode;
	glfwSetInputMode(Context::window, GLFW_CURSOR, cursorMode);
}

void Context::set_viewport() {
	glViewport(viewport_x, viewport_y, width, height);
}

void Context::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	width = width;
	height = height;
	set_viewport();
}