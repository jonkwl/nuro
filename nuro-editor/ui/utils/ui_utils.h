#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <string>
#include <glm/glm.hpp>

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
	glm::vec2 keepCursorInBounds(glm::vec4 bounds, bool& cursorMoved, float offset = 40.0f);

	// Calculates the size and offset an image can have to be displayed at the biggest size possible centered with the given aspect ratio
	void calculateAspectFitting(float aspectRatio, ImVec2& size, ImVec2& offset);

	// Returns title for window with adjusted spacing
	std::string windowTitle(const char* title);

	// Returns if the current window is focused
	bool windowFocused();

	// Returns if the current window is hovered
	bool windowHovered();

};