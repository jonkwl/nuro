#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <cstdint>
#include <vector>

#include "../src/ui/drawing/ui_text.h"

struct UIContentRect
{
	//
	// PROPERTIES
	//

	// Top left starting position of the rect in screen space coordinates
	ImVec2 position = ImVec2(0.0f, 0.0f);

	// Horizontal and vertical padding of rect content
	ImVec2 padding = ImVec2(0.0f, 0.0f);

	// Background color of rect
	uint32_t color = IM_COL32(255, 255, 255, 255);

	// Border rounding of rect in px
	float rounding = 0.0f;

	// If rect is always in foreground
	bool foreground = false;

	// If rect should have a fixed width, not determined by content
	bool useFixedWidth = false;

	// Rects width if using fixed width
	float fixedWidth = 0.0f;

	// If rect should have a fixed height, not determined by content
	bool useFixedHeight = false;

	// Rects height if using fixed height
	float fixedHeight = 0.0f;

	//
	// MEMBERS
	//

	// Draws the rect with its content
	void draw();

	// Adds given text content to the rect content
	void addContent(UIText text);

	//
	// SMOOTHING POSITION
	//
	
	// Enables smoothing for position changes
	bool positionSmoothing = true;

	// Smoothing factor when position changes
	float positionSmoothingFactor = 10.0f;

	// Last position that was cached for smoothing
	ImVec2 lastPosition;

	//
	// SMOOTHING WIDTH
	//

	// Enables smoothing for size changes
	bool sizeSmoothing = true;

	// Smoothing factor when size changes
	float sizeSmoothingFactor = 10.0f;

	// Last size that was cached for smoothing
	ImVec4 lastSizeMinMax;

private:
	// All text content elements
	std::vector<UIText> textContent;
};