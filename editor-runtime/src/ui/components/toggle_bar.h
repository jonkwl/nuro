#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <vector>
#include <cstdint>
#include <tuple>

#include "../src/ui/editor_ui.h"

struct ToggleBarStyle {
	ImVec2 padding = ImVec2(8.0f, 5.0f);
	float barRounding = 3.0f;
	float itemRounding = 3.0f;

	ImU32 text = EditorColor::text;
	ImU32 textSelected = EditorColor::text;

	ImU32 backgroundColor = EditorColor::element_transparent;
	ImU32 colorHovered = EditorColor::elementHovered_transparent;
	ImU32 colorSelected = EditorColor::elementActive_transparent;

	ImFont* font = EditorUI::getFonts().s;
};

class ToggleBar
{
public:
	ToggleBar();

	// Sets up the toggle bar with the provided presets
	void setItems(const std::vector<std::tuple<const char*, bool*>>& itemPresets);

	// Renders the toolbar
	void render(ImDrawList& drawList, ImVec2 position);

	// Returns the last evaluated size of the toggle bar
	ImVec2 getSize() const;

	// Returns a reference to the style of the toggle bar
	ToggleBarStyle& getStyle();

private:
	struct Item {
		const char* text;
		bool& value;
		ImVec2 size;

		Item(const char* text, bool& value) : text(text), value(value), size(ImVec2(0.0f, 0.0f)) {};
	};

	// Calculates the geometry of the toggle bar
	void evaluateGeometry();

private:
	std::vector<Item> items;
	ImVec2 size;
	ToggleBarStyle style;

	// Set if geometry is evaluated
	bool evaluated;
};