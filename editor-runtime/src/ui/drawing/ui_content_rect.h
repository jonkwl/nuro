#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <cstdint>
#include <vector>

#include "../src/ui/drawing/ui_text.h"

struct UIContentRect
{
	ImVec2 position = ImVec2(0.0f, 0.0f);
	ImVec2 padding = ImVec2(0.0f, 0.0f);
	uint32_t color = IM_COL32(255, 255, 255, 255);
	float rounding = 0.0f;

	void draw(ImDrawList& drawList);

	void addContent(UIText text);

private:

	std::vector<UIText> textContent;
};