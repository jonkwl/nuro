#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <vector>
#include <cstdint>
#include <tuple>

#include "../src/ui/editor_ui.h"

struct ToggleBarStyle {
	static constexpr ImVec2 padding = ImVec2(8.0f, 5.0f);
	static constexpr float barRounding = 3.0f;
	static constexpr float itemRounding = 3.0f;

	static constexpr ImU32 text = EditorColor::textFrosted;
	static constexpr ImU32 textSelected = EditorColor::text;

	static constexpr ImU32 backgroundColor = EditorColor::element_transparent;
	static constexpr ImU32 colorHovered = EditorColor::elementHovered_transparent;
	static constexpr ImU32 colorSelected = EditorColor::elementActive_transparent;
};

class ToggleBar
{
public:
	ToggleBar(const std::vector<const char*>& items);

	// Renders the toolbar
	void render(ImDrawList& drawList, ImVec2 position);

private:
	struct Item {
		const char* text;
		bool selected = false;
		ImVec2 size = ImVec2(0.0f, 0.0f);

		Item(const char* text) : text(text) {};
	};

	// Calculates the geometry of the toggle bar
	void evaluateGeometry();

private:
	ImFont* font;
	std::vector<Item> items;
	ImVec2 size;

	// Set if geometry is evaluated
	bool evaluated;
};