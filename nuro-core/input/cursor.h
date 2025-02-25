#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Cursor
{

    // Sets up cursor
	void setup();

    // Returns the current cursor position relative to window
	glm::vec2 getPosition();

    // Returns the current cursor position relative to screen
    glm::vec2 getScreenPosition();

    // Sets the cursor position relative to window
	void setPosition(glm::vec2 position);

    // Sets cursor type
	void setType(uint32_t cursorType);

    // Sets cursor mode
	void setMode(uint32_t cursorMode);

    // Centers the cursor relative to window
	void center();

};

namespace CursorType {

    static constexpr uint32_t DEFAULT = GLFW_ARROW_CURSOR;
    static constexpr uint32_t HAND = GLFW_HAND_CURSOR;
    static constexpr uint32_t POINTER = GLFW_POINTING_HAND_CURSOR;
    static constexpr uint32_t TEXT_CARET = GLFW_IBEAM_CURSOR;
    static constexpr uint32_t CROSSHAIR = GLFW_CROSSHAIR_CURSOR;
    static constexpr uint32_t RESIZE_HORIZONTAL = GLFW_HRESIZE_CURSOR;
    static constexpr uint32_t RESIZE_VERTICAL = GLFW_VRESIZE_CURSOR;
    static constexpr uint32_t RESIZE_EW = GLFW_RESIZE_EW_CURSOR;
    static constexpr uint32_t RESIZE_NS = GLFW_RESIZE_NS_CURSOR;
    static constexpr uint32_t RESIZE_NWSE = GLFW_RESIZE_NWSE_CURSOR;
    static constexpr uint32_t RESIZE_NESW = GLFW_RESIZE_NESW_CURSOR;
    static constexpr uint32_t RESIZE_ALL = GLFW_RESIZE_ALL_CURSOR;
    static constexpr uint32_t NOT_ALLOWED = GLFW_NOT_ALLOWED_CURSOR;

}

namespace CursorMode {
    static constexpr uint32_t NORMAL = GLFW_CURSOR_NORMAL;
    static constexpr uint32_t HIDDEN = GLFW_CURSOR_HIDDEN;
    static constexpr uint32_t UNAVAILABLE = GLFW_CURSOR_DISABLED;
    static constexpr uint32_t CONFINED = GLFW_CURSOR_CAPTURED;
    static constexpr uint32_t LOCKED = GLFW_CURSOR_DISABLED;
};