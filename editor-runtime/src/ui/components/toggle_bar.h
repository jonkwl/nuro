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
	ImU32 colorHovered = EditorColor::elementHovered_transparentOverlay;
	ImU32 colorSelected = EditorColor::elementActive_transparentOverlay;

	ImFont* font = EditorUI::getFonts().s;

};

class ToggleBar
{
public:
	ToggleBar();

	// Adds an item to the toggle bar
	void addItem(const char* text, bool& value, bool readonly = false);

	// Removes all items from the toggle bar
	void removeItems(std::tuple<const char*, bool*> item);

	// Updates the label text of the given item by index
	void updateItemText(const std::string& text, uint32_t itemIndex, bool updateSize = true);

	// Returns if an item defined by its index was clicked
	bool itemClicked(uint32_t index);

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

		// If set, clicking on the item will not flip its linked value
		bool readonly;

		// Clicked last frame
		bool clicked;

		// Current item size
		ImVec2 size;

		Item(const char* text, bool& value, bool readonly) : text(text), value(value), readonly(readonly), clicked(false), size(ImVec2(0.0f, 0.0f)) {};
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