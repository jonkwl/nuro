#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <cstdint>
#include <vector>

#include "ui_text.h"

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
	// FUNCTIONALITY
	//

	// Prepares the rect for the next frame
	void update();

	// Draws the rect with its content, call after update()
	void draw();

	// If mouse is currently hovering over rect, call after update()
	bool hovered();

	// If mouse clicked on rect with given mouse button, call after update()
	bool clicked(ImGuiMouseButton mouseButton);

	// If mouse double clicked on rect with given mouse button, call after update()
	bool doubleClicked(ImGuiMouseButton mouseButton);

	// If mouse dragged rect with given mouse button, call after update()
	bool dragged(ImGuiMouseButton mouseButton);

	// Adds given text to the rect content
	void addText(UIText text);

	// Returns text content by index; returns nullptr if index is invalid
	UIText* getText(uint32_t index);

	//
	// SMOOTHING
	//
	
	// Enables smoothing for various properties
	bool smoothing = true;

	// Smoothing position speed factor
	float positionSmoothingSpeed = 10.0f;

	// Smoothing size speed factor
	float sizeSmoothingSpeed = 10.0f;

	// Smoothing color speed factor
	float colorSmoothingSpeed = 10.0f;

	// Last position cached for smoothing
	ImVec2 lastPosition;

	// Last size cached for smoothing
	ImVec2 lastSize;

	// Last color cached for smoothing
	ImU32 lastColor;

private:
	// Calculate size and position
	void getGeometry(ImVec2& _position, ImVec2& _size);

	// Calculate size and position smoothed
	void getGeometrySmoothed(ImVec2& _position, ImVec2& _size);

	// Calculate smoothed color
	ImU32 getFinalColor();

	// All text content elements (text reference and last cached text size) 
	std::vector<std::tuple<UIText, ImVec2>> textContent = std::vector<std::tuple<UIText, ImVec2>>();

	// Current rect geometry

	ImVec2 rectMin;
	ImVec2 rectMax;
	ImVec2 finalPosition;
	ImVec2 finalSize;

};