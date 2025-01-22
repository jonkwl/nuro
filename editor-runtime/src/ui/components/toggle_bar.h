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

	static constexpr ImU32 text = EditorColor::text;
	static constexpr ImU32 textSelected = EditorColor::text;

	static constexpr ImU32 backgroundColor = EditorColor::element_transparent;
	static constexpr ImU32 colorHovered = EditorColor::elementHovered_transparent;
	static constexpr ImU32 colorSelected = EditorColor::elementActive_transparent;
};

class ToggleBar
{
public:
	ToggleBar();

	// Sets up the toggle bar with the provided presets
	void setItems(const std::vector<std::tuple<const char*, bool*>>& itemPresets);

	// Renders the toolbar
	void render(ImDrawList& drawList, ImVec2 position);

	// Returns the toggle bar's size
	ImVec2 getSize() const;

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
	ImFont* font;
	std::vector<Item> items;
	ImVec2 size;

	// Set if geometry is evaluated
	bool evaluated;
};