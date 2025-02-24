#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <tuple>
#include <array>

struct TitleBarStyle {

	float height = 94.0f;
	ImVec2 padding = ImVec2(15.0f, 15.0f);

	ImFont* primaryFont = nullptr;
	ImFont* secondaryFont = nullptr;
	ImFont* workspaceBarFont = nullptr;

	ImU32 primaryTextColor = IM_COL32(255, 255, 255, 255);
	ImU32 secondaryTextColor = IM_COL32(180, 180, 180, 255);

	ImU32 backgroundColor = IM_COL32(14, 14, 18, 255);
	
	float menuItemRounding = 5.0f;
	ImVec2 menuItemPadding = ImVec2(6.0f, 5.0f);
	ImU32 menuItemColor = IM_COL32(0, 0, 0, 0);
	ImU32 menuItemColorHovered = IM_COL32(38, 38, 38, 160);

	ImVec2 iconSize = ImVec2(70.0f, 70.0f);

	float borderThickness = 2.0f;
	ImU32 borderColor = IM_COL32(85, 125, 255, 120);

	float controlButtonSize = 11.0f;
	float controlButtonRounding = 8.0f;
	ImVec2 controlButtonPadding = ImVec2(9.0f, 9.0f);
	ImU32 controlButtonTextColor = IM_COL32(255, 255, 255, 180);
	ImU32 controlButtonColor = IM_COL32(38, 38, 38, 160);
	ImU32 controlButtonColorHovered = IM_COL32(60, 60, 60, 160);

	ImU32 workspaceRounding = 3.0f;
	ImU32 workspaceBackgroundColor = IM_COL32(30, 30, 34, 160);

	float workspaceItemRounding = 3.0f;
	ImVec2 workspaceItemPadding = ImVec2(26.0f, 9.0f);
	ImU32 workspaceItemColorHovered = IM_COL32(34, 34, 38, 160);
	ImU32 workspaceItemColorActive = IM_COL32(50, 50, 56, 160);

};

struct WorkspaceBarElement {

	// Configuration
	std::array<const char*, 8> items = { "Project", "General", "Level Building", "Scripting", "Lighting", "Audio", "UI Editor", "Post Processing" };
	uint32_t selection = 1;

	// Geometry cache
	ImVec2 size = ImVec2(0.0f, 0.0f);
	std::array<ImVec2, 8> itemSizes = {};

	// Set if geometry is evaluated
	bool evaluated = false;

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

	// Draws a window control button and returns if it is clicked
	bool controlButton(ImDrawList& drawList, ImVec2 position, const char* icon);

	// Draws a primary label and returns its size
	ImVec2 labelPrimary(ImDrawList& drawList, ImVec2 position, const char* text);

	// Draws a secondary label and returns its size
	ImVec2 labelSecondary(ImDrawList& drawList, ImVec2 position, const char* text);

	// Draws a menu item and returns its size and if it was pressed
	std::tuple<ImVec2, bool> menuItem(ImDrawList& drawList, ImVec2 position, const char* text);

	// Draws the workspace bar
	void placeWorkspaceBar(ImDrawList& drawList, ImVec2 position);

	// Minimizes the window
	void minimize();

	// Maximizes window
	void maximize();

	// Returns if window is currently maximized
	bool maximized();

	// Flips maximized state of window
	void flipMaximize();

	// Closes the window
	void close();

	// Current style configuration
	TitleBarStyle style;

	// Workspace bar
	WorkspaceBarElement workspaceBar;

private:
	ImVec2 titleBarPosition;
	ImVec2 titleBarSize;

	// Cache for the last mouse position relative to user screen
	glm::ivec2 lastMousePosition;

	// Set if mouse was down last frame
	bool lastMouseDown;

	// Set if mouse was on an area of the title bar allowing window movement when mouse last pressed
	bool lastMousePressedMoveSuitable;

	// Cache for mouse position relative to user screen when mouse last pressed
	glm::ivec2 lastMousePressedPosition;

	// Set if window is being moved
	bool movingWindow;

	// Set if mouse was over any interactive element this frame to prevent moving the window
	bool mouseOverElement;

private:
	// Title bar virtual window flags
	static constexpr ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoBackground | 
		ImGuiWindowFlags_NoScrollWithMouse;
};