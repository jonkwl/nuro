#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <tuple>
#include <array>

struct FooterStyle {

	float height = 36.0f;
	ImU32 backgroundColor = IM_COL32(30, 30, 30, 255);
	ImVec2 padding = ImVec2(22.0f, 0.0f);

	ImFont* primaryFont = nullptr;
	ImFont* secondaryFont = nullptr;

	ImU32 primaryTextColor = IM_COL32(255, 255, 255, 255);
	ImU32 secondaryTextColor = IM_COL32(180, 180, 180, 255);

};

class Footer
{
public:
	Footer();

	// Renders the footer in the given viewport
	void render(const ImGuiViewport& viewport);

	// Returns the style configuration of the footer
	FooterStyle& getStyle();

	// Current style configuration
	FooterStyle style;

private:
	// Renders the content of the footer
	void renderContent(ImDrawList& drawList);

private:
	ImVec2 footerPosition;
	ImVec2 footerSize;

private:
	// Footer virtual window flags
	static constexpr ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoBackground | 
		ImGuiWindowFlags_NoScrollWithMouse;
};