#include "window.h"

GLFWwindow* Window::glfw = nullptr;

int Window::viewport_x = 0;
int Window::viewport_y = 0;

int Window::width = 800;
int Window::height = 600;

std::string Window::title = "Rendering Alpha";

bool Window::fullscreen = true;

GLenum Window::cursorMode = GLFW_CURSOR_DISABLED;

void Window::setCursor(GLenum cusor_mode) {
	cursorMode = cusor_mode;
	glfwSetInputMode(Window::glfw, GLFW_CURSOR, cursorMode);
}

void Window::setViewport() {
	glViewport(viewport_x, viewport_y, width, height);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	width = width;
	height = height;
	setViewport();
}