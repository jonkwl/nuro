#include "dynamic_content.h"

#include <algorithm>
#include <time/time.h>

namespace DynamicContentUtils {

	ImVec2 lerp(const ImVec2& a, const ImVec2& b, float t) {
		return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
	}

	ImVec4 lerp(const ImVec4& a, const ImVec4& b, float t) {
		return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
	}

	ImU32 lerp(ImU32 color_a, ImU32 color_b, float t) {
		// Extract individual color channels (RGBA)
		unsigned char a_r = (color_a >> 24) & 0xFF;
		unsigned char a_g = (color_a >> 16) & 0xFF;
		unsigned char a_b = (color_a >> 8) & 0xFF;
		unsigned char a_a = color_a & 0xFF;

		unsigned char b_r = (color_b >> 24) & 0xFF;
		unsigned char b_g = (color_b >> 16) & 0xFF;
		unsigned char b_b = (color_b >> 8) & 0xFF;
		unsigned char b_a = color_b & 0xFF;

		// Interpolate each channel
		unsigned char r = static_cast<unsigned char>(a_r + t * (b_r - a_r));
		unsigned char g = static_cast<unsigned char>(a_g + t * (b_g - a_g));
		unsigned char b = static_cast<unsigned char>(a_b + t * (b_b - a_b));
		unsigned char a_ = static_cast<unsigned char>(a_a + t * (b_a - a_a));

		// Combine channels back into a single ImU32
		return (r << 24) | (g << 16) | (b << 8) | a_;
	}

	float lerp(float a, float b, float t) {
		return a + t * (b - a);
	}

	void extractRGBA(ImU32 color, uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) {
		r = (color >> IM_COL32_R_SHIFT) & 0xFF;
		g = (color >> IM_COL32_G_SHIFT) & 0xFF;
		b = (color >> IM_COL32_B_SHIFT) & 0xFF;
		a = (color >> IM_COL32_A_SHIFT) & 0xFF;
	}

	void extractRGB(ImU32 color, uint32_t& r, uint32_t& g, uint32_t& b) {
		r = (color >> IM_COL32_R_SHIFT) & 0xFF;
		g = (color >> IM_COL32_G_SHIFT) & 0xFF;
		b = (color >> IM_COL32_B_SHIFT) & 0xFF;
	}

	uint32_t extractRed(ImU32 color) {
		return (color >> IM_COL32_R_SHIFT) & 0xFF;
	}

	uint32_t extractGreen(ImU32 color) {
		return (color >> IM_COL32_G_SHIFT) & 0xFF;
	}

	uint32_t extractBlue(ImU32 color) {
		return (color >> IM_COL32_B_SHIFT) & 0xFF;
	}

	uint32_t extractAlpha(ImU32 color) {
		return (color >> IM_COL32_A_SHIFT) & 0xFF;
	}

	ImU32 blend(ImU32 source, ImU32 target) {
		uint32_t sr, sg, sb, sa, tr, tg, tb, ta;

		//
		// Extract RGBA components
		//

		sr = (source >> IM_COL32_R_SHIFT) & 0xFF;
		sg = (source >> IM_COL32_G_SHIFT) & 0xFF;
		sb = (source >> IM_COL32_B_SHIFT) & 0xFF;
		sa = (source >> IM_COL32_A_SHIFT) & 0xFF;

		// Early out if source alpha is fully opaque
		if (sa == 255) {
			return target;
		}

		tr = (target >> IM_COL32_R_SHIFT) & 0xFF;
		tg = (target >> IM_COL32_G_SHIFT) & 0xFF;
		tb = (target >> IM_COL32_B_SHIFT) & 0xFF;
		ta = (target >> IM_COL32_A_SHIFT) & 0xFF;

		// Normalize alpha values to [0, 1] and blend
		float blendedAlpha = ((sa / 255.0f) * (ta / 255.0f)) * 255.0f;

		// Clamp the result to [0, 255] and cast back
		ta = static_cast<uint32_t>(std::clamp(blendedAlpha, 0.0f, 255.0f));

		// Reconstruct the target color with the new alpha
		target = IM_COL32(tr, tg, tb, ta);

		// Return blended target color
		return target;
	}

}

void DynamicContent::draw()
{
	//
	// EARLY OUT IF WINDOW IS INVISIBLE
	//

	//

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
	// DRAW OUTLINE
	//

	if (outline) {
		drawList->AddRect(rectMin, rectMax, DynamicContentUtils::blend(color, outlineColor), rounding, ImDrawFlags_None, outlineStrength);
	}

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
		case TextAlign::LEFT:
			textPosition.x += 0.0f;
			break;
		case TextAlign::CENTER:
			textPosition.x += freeSpaceX * 0.5f;
			break;
		case TextAlign::RIGHT:
			textPosition.x += freeSpaceX;
			break;
		}
		
		// Set text position
		text.position = textPosition;

		// Blend text if needed
		uint32_t a = DynamicContentUtils::extractAlpha(finalColor);
		if (a < 255) {
			uint32_t r, g, b;
			DynamicContentUtils::extractRGB(text.color, r, g, b);
			text.color = IM_COL32(r, g, b, a);
		}

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

bool DynamicContent::hovered()
{
	return ImGui::IsMouseHoveringRect(rectMin, rectMax);
}

bool DynamicContent::clicked(ImGuiMouseButton mouseButton)
{
	return ImGui::IsMouseClicked(mouseButton) && hovered();
}

bool DynamicContent::doubleClicked(ImGuiMouseButton mouseButton)
{
	return ImGui::IsMouseDoubleClicked(mouseButton) && hovered();
}

bool DynamicContent::dragged(ImGuiMouseButton mouseButton)
{
	return ImGui::IsMouseDragging(mouseButton) && hovered();
}

void DynamicContent::addText(DynamicText text)
{
	contentDirty = true;
	textContent.push_back({ text, ImVec2(0.0f, 0.0f) });
}

DynamicText& DynamicContent::modifyText(uint32_t index)
{
	contentDirty = true;
	return std::get<0>(textContent[index]);
}

ImVec2 DynamicContent::getSize()
{
	ImVec2 _position, _size;
	getGeometry(_position, _size);
	return _size;
}

void DynamicContent::getGeometry(ImVec2& _position, ImVec2& _size)
{
	_size = ImVec2(0.0f, 0.0);

	//
	// RECALCULATE RECT CONTENT SIZE IF NEEDED
	//

	if (contentDirty) {

		// Reset content size
		contentSize = ImVec2(0.0f, 0.0f);

		// Calculate text related sizes
		for (auto& [text, textSize] : textContent) {
			// Get absolute text size
			textSize = text.getSize();

			// Update content width if it exceeds previous
			contentSize.x = std::max(contentSize.x, textSize.x);

			// Add text height to rect max
			contentSize.y += textSize.y;
		}

	}

	//
	// CALCULATE FINAL SIZE OF RECT
	//

	// First match rect size to current content size
	_size = contentSize;

	// Overwrite rect size x if using fixed width
	if (useFixedWidth) _size.x = fixedWidth;

	// Overwrite rect size y if using fixed height
	if (useFixedWidth) _size.y = fixedHeight;

	// Add total padding to rect size
	_size += padding * 2;

	//
	// CALCULATE FINAL POSITION OF RECT
	//

	// All alignments are free -> early out, pass on current position property
	if (horizontalAlignment == Horizontal::FREE && verticalAlignment == Vertical::FREE) {
		_position = position;
		return;
	}

	// Get window position and size
	ImVec2 windowPosition = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	// Calculate horizontal position based on horizontal alignment
	switch (horizontalAlignment) {
	case Horizontal::FREE:
		_position.x = position.x;
		break;
	case Horizontal::LEFT:
		_position.x = windowPosition.x + margin.x;
		break;
	case Horizontal::CENTER:
		_position.x = windowPosition.x + windowSize.x * 0.5f - _size.x * 0.5f;
		break;
	case Horizontal::RIGHT:
		_position.x = windowPosition.x + windowSize.x - _size.x - margin.x;
		break;
	}

	// Calculate vertical position based on vertical alignment
	switch (verticalAlignment) {
	case Vertical::FREE:
		_position.y = position.y;
		break;
	case Vertical::TOP:
		_position.y = windowPosition.y + margin.y;
		break;
	case Vertical::CENTER:
		_position.y = windowPosition.y + windowSize.y * 0.5f - _size.y * 0.5f;
		break;
	case Vertical::BOTTOM:
		_position.y = windowPosition.y + windowSize.y - _size.y - margin.y;
		break;
	}
	
}

void DynamicContent::getGeometrySmoothed(ImVec2& _position, ImVec2& _size)
{
	// Get non smoothed geometry
	getGeometry(_position, _size);

	// Smooth position
	ImVec2 smoothedPosition = DynamicContentUtils::lerp(lastPosition, _position, positionSmoothingSpeed * Time::deltaf());
	_position = smoothedPosition;
	lastPosition = smoothedPosition;

	// Smooth size
	ImVec2 smoothedSize = DynamicContentUtils::lerp(lastSize, _size, sizeSmoothingSpeed * Time::deltaf());
	_size = smoothedSize;
	lastSize = smoothedSize;
}

ImU32 DynamicContent::getFinalColor()
{
	ImU32 finalColor = color;

	// Smooth color
	if (smoothing) {
		ImU32 smoothedColor = DynamicContentUtils::lerp(lastColor, color, colorSmoothingSpeed * Time::deltaf());
		finalColor = smoothedColor;
		lastColor = smoothedColor;
	}

	return finalColor;
}