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

	// Horizontal and vertical padding of rect content (space inside rect between content) 
	ImVec2 padding = ImVec2(0.0f, 0.0f);

	// Horizontal and vertical margin of rect content (space outside rect when using non free alignment)
	ImVec2 margin = ImVec2(0.0f, 0.0f);

	// Background color of rect
	ImU32 color = IM_COL32(255, 255, 255, 255);

	// Border rounding of rect in px
	float rounding = 0.0f;

	// If rect should have an outline
	bool outline = false;

	// Thickness of outline rect
	float outlineStrength = 0.0f;

	// Color of outline rect
	ImU32 outlineColor;

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

	// Horizontal alignment of rect relative to parent window (use FREE for custom positioning)
	Horizontal horizontalAlignment = Horizontal::FREE;
	
	// Vertical alignment of rect relative to parent window (use FREE for custom positioning)
	Vertical verticalAlignment = Vertical::FREE;

	//
	// FUNCTIONALITY
	//

	// Draws the rect with its content
	void draw();

	// If mouse is currently hovering over rect
	bool hovered();

	// If mouse clicked on rect with given mouse button
	bool clicked(ImGuiMouseButton mouseButton);

	// If mouse double clicked on rect with given mouse button
	bool doubleClicked(ImGuiMouseButton mouseButton);

	// If mouse dragged rect with given mouse button
	bool dragged(ImGuiMouseButton mouseButton);

	// Adds given text element to the rect content
	void addText(UIText text);

	// Returns content text element by index (never cache this, only modify directly)
	UIText& modifyText(uint32_t index);

	// Computes and returns the rects total size
	ImVec2 getSize();

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

	//
	// CURRENT RECT GEOMETRY
	//

	ImVec2 rectMin;
	ImVec2 rectMax;
	ImVec2 finalPosition;
	ImVec2 finalSize;

	//
	// CURRENT RECT CONTENT GEOMETRY
	//

	bool contentDirty;
	ImVec2 contentSize;

};