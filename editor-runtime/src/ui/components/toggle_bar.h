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

	// Adds an item to the toggle bar
	void addItem(const char* text, bool& value);

	// Removes all items from the toggle bar
	void removeItems(std::tuple<const char*, bool*> item);

	// Updates the label text of the given item by index
	void updateItemText(const std::string& text, uint32_t itemIndex, bool updateSize = true);

	// Renders the toolbar
	void render(ImDrawList& drawList, ImVec2 position);

	// Returns the last evaluated size of the toggle bar
	ImVec2 getSize() const;

	// Returns a reference to the style of the toggle bar
	ToggleBarStyle& getStyle();

private:
	struct Item {
		// Item labels text
		const char* text;

		// Boolean value linked to item
		bool& value;

		// Current item size
		ImVec2 size;

		// Clicked last frame
		bool clicked;

		Item(const char* text, bool& value) : text(text), value(value), size(ImVec2(0.0f, 0.0f)), clicked(false) {};
	};

	// Calculates and updates the current geometry of the toggle bar
	void evaluateGeometry();

private:
	std::vector<Item> items;
	ImVec2 size;
	ToggleBarStyle style;

	// Set if geometry needs to be updated
	bool geometryUpdatePending;
};