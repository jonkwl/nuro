#pragma once

#include <imgui.h>
#include <vector>
#include <string>

namespace EditorUI
{
	struct Fonts
	{
		ImFont* uiRegular;
		ImFont* uiBold;
		ImFont* uiHeadline;
		ImFont* uiBig;
		ImFont* uiSmall;
		ImFont* uiIcons;
	};

	// Sets up editor ui
	void setup();

	// Begins a new frame for the editor ui
	void newFrame();

	// Renders the ui frame
	void render();

	// Generates a unique imgui id
	std::string generateId();

	// Returns all loaded fonts
	const Fonts& getFonts();

	// Sets the current editor ui cursor type
	void setCursorType(int32_t cursorType);

	// Sets the current editor ui cursor mode
	void setCursorMode(int32_t cursorMode);
};

struct EditorSizing
{
	static constexpr float regularFontSize = 14.0f;
	static constexpr float headlineFontSize = 15.0f;
	static constexpr float bigFontSize = 20.0f;
	static constexpr float smallFontSize = 12.0f;

	static constexpr float iconsRegularFontSize = 22.0f;
	static constexpr float iconsBoldFontSize = 25.0f;
	static constexpr float iconsBigFontSize = 30.0f;

	static constexpr float windowPadding = 30.0f;
	static constexpr float framePadding = 3.0f;
};

struct EditorColor
{
	static constexpr ImU32 background = IM_COL32(20, 20, 20, 242);
	static constexpr ImU32 text = IM_COL32(255, 255, 255, 255);

	static constexpr ImU32 element = IM_COL32(51, 51, 51, 255);
	static constexpr ImU32 elementHovered = IM_COL32(64, 64, 64, 255);
	static constexpr ImU32 elementActive = IM_COL32(77, 77, 77, 255);
	static constexpr ImU32 elementComponent = IM_COL32(178, 178, 178, 255);

	static constexpr ImU32 borderColor = IM_COL32(0, 0, 0, 255);
	static constexpr ImU32 selection = IM_COL32(85, 125, 255, 255);
	static constexpr ImU32 tabColor = IM_COL32(38, 38, 38, 255);
};

struct EditorFlag
{
	static constexpr ImGuiWindowFlags viewport = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
	static constexpr ImGuiWindowFlags standard = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	static constexpr ImGuiWindowFlags fixed = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
};

struct EditorFontPath
{
	static constexpr const char* normal = "../resources/fonts/Inter_18pt-Light.ttf";
	static constexpr const char* bold = "../resources/fonts/Inter_18pt-SemiBold.ttf";
	static constexpr const char* icons = "../resources/fonts/fa-solid-900.ttf";
};