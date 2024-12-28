#include "application_context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/utils/log.h"
#include "../core/time/time.h"

ApplicationContext::ApplicationContext() : window(nullptr),
configuration()
{
}

void ApplicationContext::create(Configuration _configuration)
{
	// Sync given configuration with application context instances configuration
	configuration = _configuration;

	// Start creating application context
	Log::printProcessStart("Application Context", "Creating application context...");

	// Set error callback and initialize context
	glfwSetErrorCallback(glfwErrorCallback);
	glfwInit();

	// Set versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get monitor and mode
	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Enable HDR output
	glfwWindowHint(GLFW_RED_BITS, 10);
	glfwWindowHint(GLFW_GREEN_BITS, 10);
	glfwWindowHint(GLFW_BLUE_BITS, 10);
	glfwWindowHint(GLFW_ALPHA_BITS, 2);

	// Check for fullscreen before creating window
	if (configuration.fullscreen)
	{
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		configuration.windowSize = glm::vec2(mode->width, mode->height);
		lastWindowSize = configuration.windowSize;
	}

	// Create window
	window = glfwCreateWindow(configuration.windowSize.x, configuration.windowSize.y, configuration.windowTitle.c_str(), nullptr, nullptr);

	// Center window
	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);
	int windowX = monitorX + (mode->width - configuration.windowSize.x) / 2;
	int windowY = monitorY + (mode->height - configuration.windowSize.y) / 2;
	glfwSetWindowPos(window, windowX, windowY);

	// Check for window creation success
	if (window == nullptr)
	{
		Log::printError("Application Context", "Creation of window failed");
	}

	// Load graphics backend
	loadBackend();

	// Set menubar visibility
	setMenubarVisibility(configuration.menubarVisible);

	// Set vsync
	setVSync(configuration.vsync);
}

void ApplicationContext::destroy()
{
	// Destroy window and terminate glfw
	if (window != nullptr)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

void ApplicationContext::startFrame()
{
	// Update glfw events
	glfwPollEvents();

	// Step global time
	Time::step(glfwGetTime());
}

void ApplicationContext::endFrame()
{
	// Swap gWindow buffers
	glfwSwapBuffers(window);
}

bool ApplicationContext::running()
{
	// Checks if window is supposed to close
	return !glfwWindowShouldClose(window);
}

void ApplicationContext::setFullscreen() {
	// Get video mode
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Cache current window size
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	lastWindowSize = glm::ivec2(windowWidth, windowHeight);

	// Set window to fullscreen size
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

	// Hide windows menu bar without overwriting menu bar visibility configuration
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	// Sync config
	configuration.fullscreen = true;
}

void ApplicationContext::setWindowed() {
	// Get video mode
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Set window to last cached size
	glfwSetWindowMonitor(window, monitor, 0, 0, lastWindowSize.x, lastWindowSize.y, mode->refreshRate);

	// Sync window with configurations menu bar visibility
	glfwWindowHint(GLFW_DECORATED, configuration.menubarVisible ? GLFW_TRUE : GLFW_FALSE);

	// Sync config
	configuration.windowSize = lastWindowSize;
	configuration.fullscreen = false;
}

void ApplicationContext::setMenubarVisibility(bool value)
{
	// Only apply menu bar visibility if window is not fullscreened
	if (!configuration.fullscreen) {
		glfwSetWindowAttrib(window, GLFW_DECORATED, value ? GLFW_TRUE : GLFW_FALSE);
	}

	// Sync configuration (always)
	configuration.menubarVisible = value;
}

void ApplicationContext::setVSync(bool value)
{
	// Set glfw swap interval according to value
	glfwSwapInterval(value ? 1 : 0);

	// Sync config
	configuration.vsync = value;
}

GLFWwindow* ApplicationContext::getWindow() const
{
	return window;
}

const ApplicationContext::Configuration& ApplicationContext::readConfiguration() const
{
	return configuration;
}

void ApplicationContext::glfwErrorCallback(int32_t error, const char* description)
{
	Log::printError("Application Context", "GLFW Error: " + std::to_string(error), description);
}

void ApplicationContext::loadBackend()
{
	// Load graphics api
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log::printError("Application Context", "Initialization of GLAD failed");
	}

	// Debug graphics api version
	const char* version = (const char*)glGetString(GL_VERSION);
	Log::printProcessDone("Application Context", "Initialized, OpenGL version: " + std::string(version));

	Log::printProcessDone("Application Context", "Created application context");
}
