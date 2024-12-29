#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <cstdint>
#include <string>

struct UIText
{
	UIText(ImFont* font);

	ImFont* font;
	std::string text = "Text Element";
	ImVec2 position = ImVec2(0.0f, 0.0f);
	uint32_t color = IM_COL32(0, 0, 0, 255);
	ImVec2 padding = ImVec2(0.0f, 0.0f);

	// Draws the ui text
	void draw(ImDrawList* drawList);

	// Returns the absolute size of the text in px
	ImVec2 getSize() const;
};
