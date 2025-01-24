#include "toggle_bar.h"

#include "../src/ui/editor_ui.h"

#include "../core/utils/log.h"

ToggleBar::ToggleBar() : items(),
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

	// Get style.font size
	float fontSize = style.font->FontSize;

	// Draw background
	drawList.AddRectFilled(position, position + size, style.backgroundColor, style.barRounding);

	// Draw items
	ImVec2 cursor = position + style.padding;
	for (Item& item : items) {
		// Draw text
		ImU32 textColor = item.value ? style.textSelected : style.text;
		ImVec2 textSize = style.font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, item.text);
		drawList.AddText(style.font, fontSize, cursor, textColor, item.text);

		// Evaluate item interactions
		ImVec2 itemSize = textSize + style.padding;
		ImVec2 p0 = cursor - style.padding;
		ImVec2 p1 = p0 + itemSize + style.padding;
		bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
		bool clicked = hovered && ImGui::IsMouseClicked(0);

		// Flip item value if clicked
		if (clicked) item.value = !item.value;

		// If hovered or item value set, draw item background and redraw item text
		if (item.value || hovered) {
			ImU32 color = style.colorHovered;
			if (item.value) color = style.colorSelected;
			drawList.AddRectFilled(p0, p1, color, style.itemRounding);
			drawList.AddText(style.font, fontSize, cursor, textColor, item.text);
		}

		// Advance cursor
		cursor.x += textSize.x + style.padding.x * 2.0f;
	}
}

ImVec2 ToggleBar::getSize() const
{
	return size;
}

ToggleBarStyle& ToggleBar::getStyle()
{
	return style;
}

void ToggleBar::evaluateGeometry()
{
    // Reset size and get style.font size
    size = ImVec2(0.0f, 0.0f);
    float fontSize = style.font->FontSize;

    // Calculate each items size and add it to total size
    for (Item& item : items) {
        // Calculate size of current item
        item.size = style.font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, item.text) + style.padding * 2.0f;

        // Add item size to total size
        size = item.size + ImVec2(size.x, 0.0f);
    }

	evaluated = true;
}