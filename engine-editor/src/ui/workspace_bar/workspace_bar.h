#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <tuple>
#include <array>

//
// NOT USED AT THE MOMENT! //
//

struct WorkspaceBarStyle {

	float height = 37.0f;
	ImU32 backgroundColor = IM_COL32(22, 22, 22, 255);
	ImVec2 padding = ImVec2(14.0f, 0.0f);

	ImFont* primaryFont = nullptr;
	ImFont* secondaryFont = nullptr;

	ImU32 primaryTextColor = IM_COL32(255, 255, 255, 180);
	ImU32 secondaryTextColor = IM_COL32(255, 255, 255, 150);

	float borderThickness = 1.0f;
	ImU32 borderColor = IM_COL32(0, 0, 0, 255);

};

class WorkspaceBar
{
public:
	WorkspaceBar();

	// Renders the footer in the given viewport
	void render(const ImGuiViewport& viewport, float yOffset);

	// Returns the style configuration of the footer
	WorkspaceBarStyle& getStyle();

	// Current style configuration
	WorkspaceBarStyle style;

private:
	// Renders the content of the footer
	void renderContent(ImDrawList& drawList);

private:
	ImVec2 position;
	ImVec2 size;

private:
	// Workspace bar virtual window flags
	static constexpr ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoBackground | 
		ImGuiWindowFlags_NoScrollWithMouse;
};