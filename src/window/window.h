#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <glm.hpp>

class Window
{
public:
	static GLFWwindow* glfw;

	static int width;
	static int height;

	static glm::vec2 getSize();

	static std::string title;

	static bool fullscreen;

	static GLenum cursorMode;

	static void setCursor(GLenum cusor_mode);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};