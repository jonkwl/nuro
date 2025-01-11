#include "application_context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../core/utils/log.h"
#include "../core/time/time.h"
#include "../core/input/input.h"
#include "../core/input/cursor.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/rendering/primitives/global_quad.h"

namespace ApplicationContext {

	GLFWwindow* gWindow = nullptr;
	GLFWmonitor* gMonitor = nullptr;
	Configuration gConfiguration;
	glm::ivec2 gLastWindowSize = glm::ivec2(0.0f, 0.0f);

	// Default glfw error callback
	static void _glfwErrorCallback(int32_t error, const char* description)
	{
		Log::printError("Application Context", "GLFW Error: " + std::to_string(error), description);
	}

	// Loads graphics backend
	void _loadBackend()
	{
		// Load graphics api
		glfwMakeContextCurrent(gWindow);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::printError("Application Context", "Initialization of GLAD failed");
		}

		// Debug graphics api version
		const char* version = (const char*)glGetString(GL_VERSION);
		Log::printProcessDone("Application Context", "Initialized, OpenGL version: " + std::string(version));

		Log::printProcessDone("Application Context", "Created application context");
	}

	void create(Configuration configuration)
	{
		//
		// CREATE GLFW CONTEXT
		//

		// Sync given configuration with application context instances configuration
		gConfiguration = configuration;

		// Start creating application context
		Log::printProcessStart("Application Context", "Creating application context...");

		// Set error callback and initialize context
		glfwSetErrorCallback(_glfwErrorCallback);
		glfwInit();

		// Set versions
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Get monitor and mode
		gMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(gMonitor);

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
			gLastWindowSize = configuration.windowSize;
		}

		// Create window
		gWindow = glfwCreateWindow(configuration.windowSize.x, configuration.windowSize.y, configuration.windowTitle.c_str(), nullptr, nullptr);

		// Center window
		int monitorX, monitorY;
		glfwGetMonitorPos(gMonitor, &monitorX, &monitorY);
		int windowX = monitorX + (mode->width - configuration.windowSize.x) / 2;
		int windowY = monitorY + (mode->height - configuration.windowSize.y) / 2;
		glfwSetWindowPos(gWindow, windowX, windowY);

		// Check for window creation success
		if (gWindow == nullptr)
		{
			Log::printError("Application Context", "Creation of window failed");
		}

		// Load graphics backend
		_loadBackend();

		// Set menubar visibility
		setMenubarVisibility(configuration.menubarVisible);

		// Set vsync
		setVSync(configuration.vsync);

		//
		// SETUP OTHER SYSTEMS
		//

		// Setup input and cursor
		Input::setup();
		Cursor::setup();

		// Create essential primitives
		GlobalQuad::create();
	}

	void destroy()
	{
		// Destroy window and terminate glfw
		if (gWindow != nullptr)
		{
			glfwDestroyWindow(gWindow);
			glfwTerminate();
		}
	}

	void startFrame()
	{
		// Update glfw events
		glfwPollEvents();

		// Step global time
		Time::step(glfwGetTime());

		// Update diagnostics
		Diagnostics::step();

		// Update input system
		Input::update();
	}

	void endFrame()
	{
		// Swap gWindow buffers
		glfwSwapBuffers(gWindow);
	}

	bool running()
	{
		// Checks if window is supposed to close
		return !glfwWindowShouldClose(gWindow);
	}

	void setFullscreen() {
		// Get video mode
		const GLFWvidmode* mode = glfwGetVideoMode(gMonitor);

		// Cache current window size
		int windowWidth, windowHeight;
		glfwGetWindowSize(gWindow, &windowWidth, &windowHeight);
		gLastWindowSize = glm::ivec2(windowWidth, windowHeight);

		// Set window to fullscreen size
		glfwSetWindowMonitor(gWindow, gMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		// Hide windows menu bar without overwriting menu bar visibility configuration
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		// Sync config
		gConfiguration.fullscreen = true;
	}

	void setWindowed() {
		// Get video mode
		const GLFWvidmode* mode = glfwGetVideoMode(gMonitor);

		// Set window to last cached size
		glfwSetWindowMonitor(gWindow, gMonitor, 0, 0, gLastWindowSize.x, gLastWindowSize.y, mode->refreshRate);

		// Sync window with configurations menu bar visibility
		glfwWindowHint(GLFW_DECORATED, gConfiguration.menubarVisible ? GLFW_TRUE : GLFW_FALSE);

		// Sync config
		gConfiguration.windowSize = gLastWindowSize;
		gConfiguration.fullscreen = false;
	}

	void setMenubarVisibility(bool value)
	{
		// Only apply menu bar visibility if window is not fullscreened
		if (!gConfiguration.fullscreen) {
			glfwSetWindowAttrib(gWindow, GLFW_DECORATED, value ? GLFW_TRUE : GLFW_FALSE);
		}

		// Sync configuration (always)
		gConfiguration.menubarVisible = value;
	}

	void setVSync(bool value)
	{
		// Set glfw swap interval according to value
		glfwSwapInterval(value ? 1 : 0);

		// Sync config
		gConfiguration.vsync = value;
	}

	GLFWwindow* getWindow()
	{
		return gWindow;
	}

	const Configuration& readConfiguration()
	{
		return gConfiguration;
	}

}