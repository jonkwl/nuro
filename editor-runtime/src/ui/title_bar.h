#pragma once

#include <cstdint>
#include <glm.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <tuple>
#include <array>

struct TitleBarStyle {

	float height = 70.0f;
	ImVec2 padding = ImVec2(15.0f, 15.0f);

	ImFont* primaryFont = nullptr;
	ImFont* secondaryFont = nullptr;

	ImU32 primaryTextColor = IM_COL32(255, 255, 255, 255);
	ImU32 secondaryTextColor = IM_COL32(200, 200, 200, 255);

	ImU32 backgroundColor = IM_COL32(14, 14, 18, 255);
	
	float menuItemRounding = 5.0f;
	ImVec2 menuItemPadding = ImVec2(6.0f, 5.0f);
	ImU32 menuItemColor = IM_COL32(0, 0, 0, 0);
	ImU32 menuItemColorHovered = IM_COL32(38, 38, 38, 160);

	uint32_t iconTexture = 0;
	ImVec2 iconSize = ImVec2(70.0f, 70.0f);

	float borderThickness = 2.0f;
	ImU32 borderColor = IM_COL32(85, 125, 255, 120);

};

class TitleBar
{
public:
	TitleBar();

	// Renders the titlebar in the given viewport
	void render(const ImGuiViewport& viewport);

	// Returns the style configuration of the title bar
	TitleBarStyle& getStyle();

private:
	// Renders the content of the title bar
	void renderContent(ImDrawList& drawList);

	// Performs moving the window if title bar is dragged
	void performDrag();

	// Draws a primary label and returns its size
	inline ImVec2 labelPrimary(ImDrawList& drawList, ImVec2 position, const char* text);

	// Draws a secondary label and returns its size
	inline ImVec2 labelSecondary(ImDrawList& drawList, ImVec2 position, const char* text);

	// Draws a menu item and returns its size and if it was pressed
	std::tuple<ImVec2, bool> menuItem(ImDrawList& drawList, ImVec2 position, const char* text);

	// Returns the mouse delta relative to the screen
	ImVec2 getScreenMouseDelta();

	// Current style configuration
	TitleBarStyle style;

private:
	// States

	ImVec2 titleBarPosition;
	ImVec2 titleBarSize;

	glm::ivec2 lastMousePosition;
	bool movingWindow;

private:
	// Title bar virtual window flags
	static constexpr ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoBackground | 
		ImGuiWindowFlags_NoScrollWithMouse;
};