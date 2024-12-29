#include "ui_content_rect.h"

#include "draw_internal.h"
#include "draw_utils.h"

#include <algorithm>

void UIContentRect::draw()
{
	//
	// GET DRAW LIST
	//

	ImDrawList* drawList = nullptr;
	if (foreground) {
		drawList = ImGui::GetForegroundDrawList();
	}
	else {
		drawList = ImGui::GetWindowDrawList();
	}

	//
	// GET GEOMETRY
	//

	ImVec2 finalPosition, finalSize;
	if (smoothing) {
		getGeometrySmoothed(finalPosition, finalSize);
	}
	else {
		getGeometry(finalPosition, finalSize);
	}

	//
	// CALCULATE FINAL MIN AND MAX OF RECT
	//
	
	ImVec2 rectMin = finalPosition;
	ImVec2 rectMax = finalPosition + finalSize;

	//
	// GET FINAL COLOR
	//

	ImU32 finalColor = getFinalColor();

	// 
	// DRAW RECT
	// 

	drawList->AddRectFilled(rectMin, rectMax, finalColor, rounding);

	//
	// DRAW CONTENT OF RECT

	// Initialize content cursor
	ImVec2 contentCursor = rectMin + padding;

	// Draw text content
	for (auto& [text, textSize] : textContent) {
		// Set texts final position to content cursor
		ImVec2 textPosition = contentCursor;
		
		// Adjust texts final horizontal position according to its alignment
		float freeSpaceX = finalSize.x - textSize.x - padding.x * 2;
		switch (text.alignment) {
		case TEXT_LEFT:
			textPosition.x += 0.0f;
			break;
		case TEXT_CENTERED:
			textPosition.x += freeSpaceX * 0.5f;
			break;
		case TEXT_RIGHT:
			textPosition.x += freeSpaceX;
			break;
		}
		
		// Set text position
		text.position = textPosition;

		// Draw text
		text.draw(drawList);

		// Advance cursor
		contentCursor += ImVec2(0.0f, text.getSize().y);
	}

	//
	// ADVANCE BACKEND CURSOR
	//

	ImVec2 contentRegion = ImGui::GetContentRegionAvail();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, rectMax.y - rectMin.y));
	ImGui::PopStyleVar();
}

void UIContentRect::addContent(UIText text)
{
	textContent.push_back(std::make_tuple(text, ImVec2(0.0f, 0.0f)));
}

void UIContentRect::getGeometry(ImVec2& finalPosition, ImVec2& finalSize)
{
	// Pass on rect position
	finalPosition = position;

	//
	// CALCULATE FINAL SIZE OF RECT
	//

	finalSize = ImVec2(0.0f, 0.0);
	float contentWidth = 0.0f;

	// Calculate text related sizes
	for (auto& [text, textSize] : textContent) {
		// Get absolute text size
		textSize = text.getSize();

		// Update content width if it exceeds previous
		contentWidth = std::max(contentWidth, textSize.x);

		// Add text height to rect max
		finalSize.y += textSize.y;
	}

	// Set rect size to content width
	finalSize.x = contentWidth;

	// Overwrite rect size x if using fixed width
	if (useFixedWidth) finalSize.x = fixedWidth;

	// Overwrite rect size y if using fixed height
	if (useFixedWidth) finalSize.y = fixedHeight;

	// Add total padding to rect size
	finalSize += padding * 2;
}

void UIContentRect::getGeometrySmoothed(ImVec2& finalPosition, ImVec2& finalSize)
{
	// Get non smoothed geometry
	getGeometry(finalPosition, finalSize);

	// Smooth position
	ImVec2 smoothedPosition = DrawUtils::lerp(lastPosition, finalPosition, positionSmoothingSpeed * delta);
	finalPosition = smoothedPosition;
	lastPosition = smoothedPosition;

	// Smooth size
	ImVec2 smoothedSize = DrawUtils::lerp(lastSize, finalSize, sizeSmoothingSpeed * delta);
	finalSize = smoothedSize;
	lastSize = smoothedSize;
}

ImU32 UIContentRect::getFinalColor()
{
	ImU32 finalColor = color;

	// Smooth color
	if (smoothing) {
		ImU32 smoothedColor = DrawUtils::lerp(lastColor, color, colorSmoothingSpeed * delta);
		finalColor = smoothedColor;
		lastColor = smoothedColor;
	}

	return finalColor;
}
