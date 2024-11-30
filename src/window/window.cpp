#include "window.h"

GLFWwindow* Window::glfw = nullptr;

int Window::width = 1200;
int Window::height = 800;

std::string Window::title = "Rendering Alpha";

bool Window::fullscreen = false;

GLenum Window::cursorMode = GLFW_CURSOR_DISABLED;

glm::vec2 Window::getSize()
{
	return glm::vec2(width, height);
}

void Window::setCursor(GLenum cusor_mode)
{
	cursorMode = cusor_mode;
	glfwSetInputMode(Window::glfw, GLFW_CURSOR, cursorMode);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int _width, int _height)
{
	width = _width;
	height = _height;
}