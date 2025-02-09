#include "dynamic_text.h"

DynamicText::DynamicText(ImFont* font) : font(font)
{
}

void DynamicText::draw(ImDrawList* drawList)
{
	// Calculate draw position with text position and padding
	ImVec2 drawPosition = position + padding;

	// Add to draw list
	drawList->AddText(font, font->FontSize, drawPosition, color, text.c_str());
}

ImVec2 DynamicText::getSize() const
{
	// Get base size from text displayed
	ImVec2 size = font->CalcTextSizeA(font->FontSize, FLT_MAX, -1.0f, text.c_str());
	
	// Add total padding to size
	size += padding * 2;

	// Return size
	return size;
}