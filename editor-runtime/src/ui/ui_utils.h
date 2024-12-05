#pragma once

#include <imgui.h>
#include <glm.hpp>

namespace UIUtils
{
	// Lightens color by given amount
	ImVec4 lighten(ImVec4 color, float amount);
	ImU32 lighten(ImU32 color, float amount);

	// Darkens color by given amount
	ImVec4 darken(ImVec4 color, float amount);
	ImU32 darken(ImU32 color, float amount);

	// Returns the relative scroll value of current ui child rendering
	float getChildScrollValue();

	// Keeps the cursor in the given boundaries, returns the new cursor position and sets if the cursor was positioned by method
	glm::vec2 keepCursorInBounds(glm::vec4 bounds, bool& positionedCursor);

};