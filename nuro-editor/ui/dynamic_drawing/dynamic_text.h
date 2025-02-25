#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <cstdint>
#include <string>

#include "enums.h"

class DynamicText
{
public:
	DynamicText(ImFont* font);

	//
	// PROPERTIES
	//

	// Pointer to font text is using
	ImFont* font;

	// Text value of element
	std::string text;

	// Position of text (may be controlled by parent)
	ImVec2 position = ImVec2(0.0f, 0.0f);

	// Color of text
	ImU32 color = IM_COL32(0, 0, 0, 255);

	// Horizontal and vertical padding of text
	ImVec2 padding = ImVec2(0.0f, 0.0f);

	// Text alignment within parent component if it has any
	TextAlign alignment = TextAlign::LEFT;

	//
	// VIRTUAL CONTENT ELEMENT FUNCTIONS
	//

	// Draws the ui text
	void draw(ImDrawList* drawList);

	// Returns the absolute size of the text in px
	ImVec2 getSize() const;
};
