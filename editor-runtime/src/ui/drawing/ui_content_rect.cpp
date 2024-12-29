#include "ui_content_rect.h"

#include "draw_internal.h"
#include "draw_utils.h"

#include <algorithm>

void UIContentRect::draw()
{
	// Get draw list according to foreground setting
	ImDrawList* drawList = nullptr;
	if (foreground) {
		drawList = ImGui::GetForegroundDrawList();
	}
	else {
		drawList = ImGui::GetWindowDrawList();
	}

	// Calculate and use smoothed position if needed
	if (positionSmoothing) {
		ImVec2 smoothedPosition = DrawUtils::lerp(lastPosition, position, positionSmoothingFactor * delta);
		position = smoothedPosition;
		lastPosition = position;
	}

	// Initialize
	ImVec2 rectMin = position;
	ImVec2 rectMax = position;
	float contentWidth = 0.0f;

	// Calculate text related sizes
	for (UIText text : textContent) {
		// Get absolute text size
		ImVec2 size = text.getSize();

		// Update content width if it exceeds previous
		contentWidth = std::max(contentWidth, size.x);

		// Add text height to rect max
		rectMax.y += size.y;
	}

	// Add content width to rect max
	rectMax.x += contentWidth;

	// Overwrite rect max x if using fixed width
	if (useFixedWidth) rectMax.x = position.x + fixedWidth;

	// Overwrite rect max y if using fixed height
	if (useFixedWidth) rectMax.y = position.y + fixedHeight;

	// Add total padding to rect max
	rectMax += padding * 2;

	// Draw rect
	drawList->AddRectFilled(rectMin, rectMax, color, rounding);

	// Initialize content cursor
	ImVec2 contentCursor = rectMin + padding;

	// Draw text content
	for (UIText text : textContent) {
		// Set text position
		text.position = contentCursor;

		// Draw text
		text.draw(drawList);

		// Advance cursor
		contentCursor += ImVec2(0.0f, text.getSize().y);
	}

	// Advance backend cursor 
	ImVec2 contentRegion = ImGui::GetContentRegionAvail();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, rectMax.y - rectMin.y));
	ImGui::PopStyleVar();
}

void UIContentRect::addContent(UIText text)
{
	textContent.push_back(text);
}