#include "toggle_bar.h"

#include "../src/ui/editor_ui.h"

#include "../core/utils/log.h"

ToggleBar::ToggleBar() : font(EditorUI::getFonts().s),
items(),
size(ImVec2(0.0f, 0.0f)),
evaluated(false)
{
}

void ToggleBar::setItems(const std::vector<std::tuple<const char*, bool*>>& itemPresets)
{
	// Create items by provided item presets
	items.clear();
	for (auto& [name, value] : itemPresets) {
		items.push_back(Item{ name, *value });
	}

	// Make sure geometry will be (re-)evaluated
	evaluated = false;
}

void ToggleBar::render(ImDrawList& drawList, ImVec2 position)
{
	// Evaluate geometry if needed
	if (!evaluated) evaluateGeometry();

	// Get font size
	float fontSize = font->FontSize;

	// Draw background
	drawList.AddRectFilled(position, position + size, ToggleBarStyle::backgroundColor, ToggleBarStyle::barRounding);

	// Draw items
	ImVec2 cursor = position + ToggleBarStyle::ToggleBarStyle::padding;
	for (Item& item : items) {
		// Draw text
		ImU32 textColor = item.value ? ToggleBarStyle::textSelected : ToggleBarStyle::text;
		ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, item.text);
		drawList.AddText(font, fontSize, cursor, textColor, item.text);

		// Evaluate item interactions
		ImVec2 itemSize = textSize + ToggleBarStyle::padding;
		ImVec2 p0 = cursor - ToggleBarStyle::padding;
		ImVec2 p1 = p0 + itemSize + ToggleBarStyle::padding;
		bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
		bool clicked = hovered && ImGui::IsMouseClicked(0);

		// Flip item value if clicked
		if (clicked) item.value = !item.value;

		// If hovered or item value set, draw item background and redraw item text
		if (item.value || hovered) {
			ImU32 color = ToggleBarStyle::colorHovered;
			if (item.value) color = ToggleBarStyle::colorSelected;
			drawList.AddRectFilled(p0, p1, color, ToggleBarStyle::itemRounding);
			drawList.AddText(font, fontSize, cursor, textColor, item.text);
		}

		// Advance cursor
		cursor.x += textSize.x + ToggleBarStyle::padding.x * 2.0f;
	}
}

ImVec2 ToggleBar::getSize() const
{
	return size;
}

void ToggleBar::evaluateGeometry()
{
    // Reset size and get font size
    size = ImVec2(0.0f, 0.0f);
    float fontSize = font->FontSize;

    // Calculate each items size and add it to total size
    for (Item& item : items) {
        // Calculate size of current item
        item.size = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, item.text) + ToggleBarStyle::padding * 2.0f;

        // Add item size to total size
        size = item.size + ImVec2(size.x, 0.0f);
    }

	evaluated = true;
}