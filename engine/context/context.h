#pragma once

#include <string>
#include <GLFW/glfw3.h>

class Context
{
public:
	static GLFWwindow* window;

	static int viewport_x;
	static int viewport_y;

	static int width;
	static int height;

	static std::string title;

	static bool fullscreen;

	static GLenum cursorMode;

	static void setCursor(GLenum cusor_mode);
};