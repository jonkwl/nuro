#pragma once

#include <cstdint>
#include <glm.hpp>

namespace Cursor
{

    // Sets up cursor
	void setup();

    // Returns the current cursor position relative to window
	glm::vec2 getPosition();

    // Sets the cursor position relative to window
	void setPosition(glm::vec2 position);

    // Sets cursor type
	void setType(int32_t cursorType);

    // Sets cursor mode
	void setMode(int32_t cursorMode);

    // Centers the cursor relative to window
	void center();

};

namespace CursorType {
	static constexpr int32_t DEFAULT = 0x00036001;
	static constexpr int32_t POINTER = 0x00036004;
	static constexpr int32_t TEXT = 0x00036002;
	static constexpr int32_t CROSSHAIR = 0x00036003;
	static constexpr int32_t RESIZE_HORIZONTAL = 0x00036005;
	static constexpr int32_t RESIZE_VERTICAL = 0x00036006;
};

namespace CursorMode
{
	static constexpr int32_t NORMAL = 0x00036006;
	static constexpr int32_t HIDDEN = 0x00034002;
	static constexpr int32_t UNAVAILABLE = 0x00034002;
	static constexpr int32_t CONFINED = 0x00034004;
	static constexpr int32_t LOCKED = 0x00034003;
};
