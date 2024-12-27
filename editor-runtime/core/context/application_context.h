#pragma once

#include <glm.hpp>
#include <string>

#include "../core/backend/api.h"

struct GLFWwindow;

class ApplicationContext
{
public:
	// Represents the configuration settings for an application context
	struct Configuration {
		API api = API::NONE;

		std::string windowTitle = "Runtime";
		glm::vec2 windowSize = glm::vec2(1280.0f, 720.0f);
		bool fullscreen = false;

		bool vsync = true;
	};

public:
	ApplicationContext();

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

	// Updates the vsync configuration option
	void setVSync(bool value);

	// Returns a pointer to the applications window
	GLFWwindow* getWindow() const;

	// Returns a read-only reference to the currently active configuration
	const Configuration& readConfiguration() const;

private:
	static void glfwErrorCallback(int32_t error, const char* description);

private:
	GLFWwindow* window;
	Configuration configuration;

};