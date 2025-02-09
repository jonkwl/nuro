#include "toggle_bar.h"

#include "../src/ui/editor_ui.h"

#include "../src/core/utils/console.h"

ToggleBar::ToggleBar() : items(),
size(ImVec2(0.0f, 0.0f)),
geometryUpdatePending(false)
{
}

void ToggleBar::addItem(const char* text, bool& value, bool readonly)
{
	// Add item to items
	items.push_back(Item(text, value, readonly, style));

	// Make sure geometry will be (re-)evaluated
	geometryUpdatePending = true;
}

void ToggleBar::removeItems(std::tuple<const char*, bool*> item)
{
	// Clear all items
	items.clear();
}

void ToggleBar::updateItemText(const std::string& text, uint32_t itemIndex, bool updateSize)
{
	EDITOR_ASSERT(itemIndex < items.size(), "Invalid item index provided");

	// Change text of item
	items[itemIndex].text = text.c_str();

	// Update size if set
	if (updateSize) geometryUpdatePending = true;
}

bool ToggleBar::itemClicked(uint32_t itemIndex)
{
	EDITOR_ASSERT(itemIndex < items.size(), "Invalid item index provided");

	// Return clicked state of item
	return items[itemIndex].clicked;
}

ToggleBar::ItemStyle& ToggleBar::getItemStyle(uint32_t itemIndex)
{
	EDITOR_ASSERT(itemIndex < items.size(), "Invalid item index provided");

	// Return mutable reference to style of item
	return items[itemIndex].style;
}

void ToggleBar::render(ImDrawList& drawList, ImVec2 position)
{
	// Evaluate geometry if needed
	if (geometryUpdatePending) evaluateGeometry();

	// Get style.font size
	float fontSize = style.font->FontSize;

	// Draw background
	drawList.AddRectFilled(position, position + size, style.backgroundColor, style.barRounding);

	// Draw items
	ImVec2 cursor = position + style.padding;
	for (Item& item : items) {
		// Make sure item isn't clicked by default
		item.clicked = false;

		// Draw text
		ImU32 textColor = item.value ? style.textSelectedColor : style.textColor;
		ImVec2 textSize = style.font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, item.text);
		drawList.AddText(style.font, fontSize, cursor, textColor, item.text);

		// Evaluate item interactions
		ImVec2 itemSize = textSize + style.padding;
		ImVec2 p0 = cursor - style.padding;
		ImVec2 p1 = p0 + itemSize + style.padding;
		bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
		bool clicked = hovered && ImGui::IsMouseClicked(0);

		// If item was clicked
		if (clicked) {
			// Set item to be clicked
			item.clicked = true;

			// Flip value if item isn't readonly
			if(!item.readonly) item.value = !item.value;
		}

		// If hovered or item value set, draw item background and redraw item text
		if (item.value || hovered) {
			ImU32 color = item.style.hoveredColor;
			if (item.value) color = item.style.selectedColor;
			drawList.AddRectFilled(p0, p1, color, item.style.rounding);
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

	// No geometry update pending anymore
	geometryUpdatePending = false;
}