#pragma once

#include <glm/glm.hpp>
#include <string>

#include <backend/api.h>
#include <resource/resource_loader.h>

struct GLFWwindow;
struct GLFWmonitor;

namespace ApplicationContext
{
	// Represents the configuration settings for an application context
	struct Configuration {
		API api = API::NONE;
		std::string windowTitle = "Runtime";
		glm::ivec2 windowSize = glm::vec2(1280, 720);
		glm::ivec2 windowPosition = glm::vec2(0, 0);
		bool fullscreen = false;
		bool menubarVisible = true;
		bool vsync = true;
		bool resizeable = true;
		bool visible = true;
	};

	// Creates application context with given configuration
	void create(Configuration configuration);

	// Destroys application context
	void destroy();

	// Begins a new frame
	void nextFrame();

	// Ends a frame
	void endFrame();

	// Returns if application is still running
	bool running();

	// Resizes window
	void resizeWindow(glm::ivec2 size);

	// Maximizes window
	void maximizeWindow();

	// Minimizes (iconifies) window
	void minimizeWindow();

	// Sets window to fullscreen
	void setFullscreen();

	// Sets window to windowed with last saved window size
	void setWindowed();

	// Sets if window is resizeable
	void setResizeable(bool value);

	// Updates the windows menu bar visibility
	void setMenubarVisibility(bool value);

	// Updates the vsync configuration option
	void setVSync(bool value);

	// Shows or hides the window
	void setVisible(bool value);

	// Sets position of window on the screen
	void setPosition(glm::ivec2 position);

	// Returns position of window on the screen
	glm::ivec2 getPosition();

	// Returns size of screen
	glm::ivec2 getScreenSize();

	// Returns a pointer to the applications window
	GLFWwindow* getWindow();

	// Returns a read-only reference to the currently active configuration
	const Configuration& readConfiguration();

	// Returns the global resource loader
	ResourceLoader& getResourceLoader();

};