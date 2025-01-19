#pragma once

#include <glm.hpp>
#include <string>

#include "../core/backend/api.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace ApplicationContext
{
	// Represents the configuration settings for an application context
	struct Configuration {
		API api = API::NONE;
		std::string windowTitle = "Runtime";
		glm::ivec2 windowSize = glm::vec2(1280, 720);
		bool fullscreen = false;
		bool menubarVisible = true;
		bool vsync = true;
	};

	// Creates application context with given configuration
	void create(Configuration configuration);

	// Destroys application context
	void destroy();

	// Begins a new frame
	void startFrame();

	// Ends a frame
	void endFrame();

	// Returns if application is still running
	bool running();

	// Resizes window
	void resizeWindow(glm::ivec2 size);

	// Maximizes window
	void maximizeWindow();

	// Sets window to fullscreen
	void setFullscreen();

	// Sets window to windowed with last saved window size
	void setWindowed();

	// Updates the windows menu bar visibility
	void setMenubarVisibility(bool value);

	// Updates the vsync configuration option
	void setVSync(bool value);

	// Returns a pointer to the applications window
	GLFWwindow* getWindow();

	// Returns a read-only reference to the currently active configuration
	const Configuration& readConfiguration();

};