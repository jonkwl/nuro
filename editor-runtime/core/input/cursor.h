#pragma once

#include <cstdint>
#include <glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Cursor
{

	void setContext(GLFWwindow* window); // Set context for cursor system

	glm::vec2 getPosition(); // Return the current cursor position relative to window
	void setPosition(glm::vec2 position); // Set the cursor position relative to window

	void setType(int32_t cursorType); // Set cursor type
	void setMode(int32_t cursorMode); // Set cursor mode

	void center(); // Center the cursor relative to window

};

namespace CursorType {
	static constexpr int32_t DEFAULT = GLFW_ARROW_CURSOR;
	static constexpr int32_t POINTER = GLFW_HAND_CURSOR;
	static constexpr int32_t TEXT = GLFW_IBEAM_CURSOR;
	static constexpr int32_t CROSSHAIR = GLFW_CROSSHAIR_CURSOR;
	static constexpr int32_t RESIZE_HORIZONTAL = GLFW_HRESIZE_CURSOR;
	static constexpr int32_t RESIZE_VERTICAL = GLFW_VRESIZE_CURSOR;
};

namespace CursorMode
{
	static constexpr int32_t NORMAL = GLFW_CURSOR_NORMAL;
	static constexpr int32_t HIDDEN = GLFW_CURSOR_HIDDEN;
	static constexpr int32_t UNAVAILABLE = GLFW_CURSOR_UNAVAILABLE;
	static constexpr int32_t CONFINED = GLFW_CURSOR_CAPTURED;
	static constexpr int32_t LOCKED = GLFW_CURSOR_DISABLED;
};