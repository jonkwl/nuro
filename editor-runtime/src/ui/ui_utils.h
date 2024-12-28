#pragma once

#include <imgui.h>
#include <glm.hpp>

namespace UIUtils
{
	// Lightens color by given amount
	ImU32 lighten(ImU32 color, float amount);

	// Darkens color by given amount
	ImU32 darken(ImU32 color, float amount);

	// Lerps between given colors
	ImVec4 lerpColors(const ImVec4& a, const ImVec4& b, float t);

	// Returns the relative scroll value of current ui child rendering
	float getChildScrollValue();

	// Keeps the cursor in the given boundaries, returns the new cursor position and sets if the cursor was positioned by method
	glm::vec2 keepCursorInBounds(glm::vec4 bounds, bool& positionedCursor, float offset = 40.0f);

	// Size: Maximum size an image can have with a given aspect ratio in the current content avail
	// Offset: Offset for image to be centered
	void calculateAspectFitting(float aspectRatio, ImVec2& size, ImVec2& offset);

	// Returns title for window with adjusted spacing
	const char* windowTitle(const char* title);

};