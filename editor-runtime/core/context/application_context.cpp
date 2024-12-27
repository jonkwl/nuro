#include "application_context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/utils/log.h"

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

	// Enable HDR output
	glfwWindowHint(GLFW_RED_BITS, 10);
	glfwWindowHint(GLFW_GREEN_BITS, 10);
	glfwWindowHint(GLFW_BLUE_BITS, 10);
	glfwWindowHint(GLFW_ALPHA_BITS, 2);

	// Check for fullscreen
	if (configuration.fullscreen)
	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		configuration.windowSize = glm::vec2(mode->width, mode->height);
	}

	// Create window
	window = glfwCreateWindow(configuration.windowSize.x, configuration.windowSize.y, configuration.windowTitle.c_str(), nullptr, nullptr);

	if (window == nullptr)
	{
		Log::printError("Application Context", "Creation of window failed");
	}

	// Load graphics api
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log::printError("Application Context", "Initialization of GLAD failed");
	}

	// Set vsync
	setVSync(configuration.vsync);

	// Debug graphics api version
	const char* version = (const char*)glGetString(GL_VERSION);
	Log::printProcessDone("Application Context", "Initialized, OpenGL version: " + std::string(version));

	Log::printProcessDone("Application Context", "Created application context");
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