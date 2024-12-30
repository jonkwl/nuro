#include "ui_content_rect.h"

#include "draw_utils.h"
#include "draw_collection.h"
#include "draw_definitions.h"

#include <algorithm>

void UIContentRect::update()
{
	//
	// UPDATE GEOMETRY, GET FINAL POSITION AND SIZE
	//

	if (smoothing) {
		getGeometrySmoothed(finalPosition, finalSize);
	}
	else {
		getGeometry(finalPosition, finalSize);
	}

	//
	// CALCULATE FINAL RECT MIN AND MAX
	//

	rectMin = finalPosition;
	rectMax = finalPosition + finalSize;
}

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
		case ALIGN_LEFT:
			textPosition.x += 0.0f;
			break;
		case ALIGN_CENTER:
			textPosition.x += freeSpaceX * 0.5f;
			break;
		case ALIGN_RIGHT:
			textPosition.x += freeSpaceX;
			break;
		}
		
		// Set text position
		text.position = textPosition;

		// Scale text alpha by final color alpha
		uint32_t r, g, b, a;
		DrawUtils::extractRGB(text.color, r, g, b);
		a = DrawUtils::extractAlpha(finalColor);
		text.color = IM_COL32(r, g, b, a);

		// Draw text
		text.draw(drawList);

		// Advance cursor
		contentCursor.y += textSize.y;
	}

	//
	// ADVANCE BACKEND CURSOR IF NOT IN FOREGROUND
	//

	if (!foreground) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::Dummy(ImVec2(0.0f, rectMax.y - rectMin.y));
		ImGui::PopStyleVar();
	}
}

bool UIContentRect::hovered()
{
	return ImGui::IsMouseHoveringRect(rectMin, rectMax);
}

bool UIContentRect::clicked(ImGuiMouseButton mouseButton)
{
	return ImGui::IsMouseClicked(mouseButton) && hovered();
}

bool UIContentRect::doubleClicked(ImGuiMouseButton mouseButton)
{
	return ImGui::IsMouseDoubleClicked(mouseButton) && hovered();
}

bool UIContentRect::dragged(ImGuiMouseButton mouseButton)
{
	return ImGui::IsMouseDragging(mouseButton) && hovered();
}

void UIContentRect::addText(UIText text)
{
	textContent.push_back(std::make_tuple(text, ImVec2(0.0f, 0.0f)));
}

UIText* UIContentRect::getText(uint32_t index)
{
	if (textContent.size() - 1 < index) {
		return nullptr;
	}
	return &std::get<0>(textContent[index]);
}

ImVec2 UIContentRect::getSize()
{
	ImVec2 _position, _size;
	getGeometry(_position, _size);
	return _size;
}

void UIContentRect::getGeometry(ImVec2& _position, ImVec2& _size)
{
	// Pass on rect position
	_position = position;

	//
	// CALCULATE FINAL SIZE OF RECT
	//

	_size = ImVec2(0.0f, 0.0);
	float contentWidth = 0.0f;

	// Calculate text related sizes
	for (auto& [text, textSize] : textContent) {
		// Get absolute text size
		textSize = text.getSize();

		// Update content width if it exceeds previous
		contentWidth = std::max(contentWidth, textSize.x);

		// Add text height to rect max
		_size.y += textSize.y;
	}

	// Set rect size to content width
	_size.x = contentWidth;

	// Overwrite rect size x if using fixed width
	if (useFixedWidth) _size.x = fixedWidth;

	// Overwrite rect size y if using fixed height
	if (useFixedWidth) _size.y = fixedHeight;

	// Add total padding to rect size
	_size += padding * 2;
}

void UIContentRect::getGeometrySmoothed(ImVec2& _position, ImVec2& _size)
{
	// Get non smoothed geometry
	getGeometry(_position, _size);

	// Smooth position
	ImVec2 smoothedPosition = DrawUtils::lerp(lastPosition, _position, positionSmoothingSpeed * delta);
	_position = smoothedPosition;
	lastPosition = smoothedPosition;

	// Smooth size
	ImVec2 smoothedSize = DrawUtils::lerp(lastSize, _size, sizeSmoothingSpeed * delta);
	_size = smoothedSize;
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
