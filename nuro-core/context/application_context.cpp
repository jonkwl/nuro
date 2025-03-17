#include "application_context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>

#include <ecs/ecs.h>
#include <time/time.h>
#include <input/input.h>
#include <input/cursor.h>
#include <utils/console.h>
#include <diagnostics/diagnostics.h>
#include <rendering/primitives/global_quad.h>

namespace ApplicationContext {

	// Global configuration
	Configuration gConfiguration;

	// Global window context variables
	GLFWwindow* gWindow = nullptr;
	GLFWmonitor* gMonitor = nullptr;
	glm::ivec2 gLastWindowSize = glm::ivec2(0.0f, 0.0f);

	// Global resource manager
	ResourceManager gResourceManager;

	// Default glfw error callback
	static void _glfwErrorCallback(int32_t error, const char* description)
	{
		Console::out::error("Application Context", "GLFW Error: " + std::to_string(error), description);
	}

	// Loads graphics backend
	void _loadBackend()
	{
		// Load graphics api
		glfwMakeContextCurrent(gWindow);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Console::out::error("Application Context", "Initialization of GLAD failed");
		}

		// Debug graphics api version
		const char* version = (const char*)glGetString(GL_VERSION);
		Console::out::info("Application Context", "Initialized, OpenGL version: " + std::string(version));
	}

	void create(Configuration configuration)
	{
		//
		// CREATE GLFW CONTEXT
		//

		// Sync given configuration with application context instances configuration
		gConfiguration = configuration;

		// Start creating application context
		Console::out::start("Application Context", "Creating application context");

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

		// Hide window initially if configured
		if (!gConfiguration.visible) {
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		}

		// Create window
		gWindow = glfwCreateWindow(configuration.windowSize.x, configuration.windowSize.y, configuration.windowTitle.c_str(), nullptr, nullptr);

		// Center window
		int monitorX, monitorY;
		glfwGetMonitorPos(gMonitor, &monitorX, &monitorY);
		int windowX = monitorX + (mode->width - configuration.windowSize.x) / 2;
		int windowY = monitorY + (mode->height - configuration.windowSize.y) / 2;
		glfwSetWindowPos(gWindow, windowX, windowY);
		gConfiguration.windowPosition = glm::ivec2(windowX, windowY);

		// Check for window creation success
		if (gWindow == nullptr)
		{
			Console::out::error("Application Context", "Creation of window failed");
		}

		// Load graphics backend
		_loadBackend();

		// Set menubar visibility
		setMenubarVisibility(configuration.menubarVisible);

		// Set vsync
		setVSync(configuration.vsync);

		// Set resizeable
		setResizeable(configuration.resizeable);

		glfwHideWindow(gWindow);

		//
		// SETUP OTHER SYSTEMS
		//

		// Setup ECS
		entt::locator<ECS>::emplace();

		// Setup input and cursor
		Input::setup();
		Cursor::setup();

		// Create essential primitives
		GlobalQuad::create();

		Console::out::done("Application Context", "Created application context");
	}

	void destroy()
	{
		// Destroy window and terminate glfw
		if (gWindow != nullptr)
		{
			glfwSetWindowShouldClose(gWindow, GL_TRUE);
			glfwDestroyWindow(gWindow);
			glfwTerminate();
		}
	}

	void nextFrame()
	{
		// Update glfw events
		glfwPollEvents();

		// Make global resource loader dispatch next pending resource to gpu
		gResourceManager.dispatchNext();

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

	void resizeWindow(glm::ivec2 size)
	{
		// Resize GLFW window
		glfwSetWindowSize(gWindow, size.x, size.y);

		// Get monitor workarea
		int monitorX, monitorY, monitorWidth, monitorHeight;
		glfwGetMonitorWorkarea(gMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

		// Calculate centered position
		int posX = monitorX + (monitorWidth - size.x) / 2;
		int posY = monitorY + (monitorHeight - size.y) / 2;

		// Center window position
		glfwSetWindowPos(gWindow, posX, posY);

		// Sync config
		gConfiguration.windowSize = size;
		gConfiguration.windowPosition = glm::ivec2(posX, posY);
	}

	void maximizeWindow()
	{
		// Get work area of monitor
		int monitorX, monitorY, monitorWidth, monitorHeight;
		glfwGetMonitorWorkarea(gMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

		// Set the window size to maximum possible
		glfwSetWindowSize(gWindow, monitorWidth, monitorHeight);

		// Set window position to top-left corner of work area
		glfwSetWindowPos(gWindow, monitorX, monitorY);

		// Sync config
		gConfiguration.windowSize = glm::ivec2(monitorWidth, monitorHeight);
		gConfiguration.windowPosition = glm::ivec2(monitorX, monitorY);
	}

	void minimizeWindow()
	{
		// Iconify window
		glfwIconifyWindow(gWindow);
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

	void setResizeable(bool value)
	{
		// Only change if window is not fullscreened
		if (!gConfiguration.fullscreen) {
			glfwWindowHint(GLFW_RESIZABLE, value ? GLFW_TRUE : GL_FALSE);
		}

		// Sync config
		gConfiguration.resizeable = value;
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

	void setVisible(bool value)
	{
		// Show or hide window based on value
		if (value) {
			glfwShowWindow(gWindow);
		}
		else {
			glfwHideWindow(gWindow);
		}

		// Sync config
		gConfiguration.visible = value;
	}

	void setPosition(glm::ivec2 position)
	{
		// Set window position
		glfwSetWindowPos(gWindow, position.x, position.y);

		// Sync configuration
		gConfiguration.windowPosition = position;
	}

	glm::ivec2 getPosition()
	{
		// Receive and return window position
		int x, y;
		glfwGetWindowPos(gWindow, &x, &y);
		return glm::ivec2(x, y);
	}

	glm::ivec2 getScreenSize()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(gMonitor);
		return glm::ivec2(mode->width, mode->height);
	}

	GLFWwindow* getWindow()
	{
		return gWindow;
	}

	const Configuration& readConfiguration()
	{
		return gConfiguration;
	}

	ResourceManager& resourceManager()
	{
		return gResourceManager;
	}

}