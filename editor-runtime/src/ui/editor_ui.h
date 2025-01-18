#pragma once

#include <imgui.h>
#include <vector>
#include <string>

namespace EditorUI
{
	struct Fonts
	{
		// Paragraph
		ImFont* p;
		ImFont* p_bold;

		// Headline 1
		ImFont* h1;
		ImFont* h1_bold;

		// Headline 2
		ImFont* h2;
		ImFont* h2_bold;

		// Headline 3
		ImFont* h3;
		ImFont* h3_bold;

		// Headline 4
		ImFont* h4;
		ImFont* h4_bold;
		
		// Small
		ImFont* s;
		ImFont* s_bold;

		// Icons
		ImFont* icons;
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
	static constexpr float p_FontSize = 14.0f;
	static constexpr float h1_FontSize = 30.0f;
	static constexpr float h2_FontSize = 26.0f;
	static constexpr float h3_FontSize = 20.0f;
	static constexpr float h4_FontSize = 15.0f;
	static constexpr float s_FontSize = 12.0f;

	static constexpr float p_IconSize = 22.0f;
	static constexpr float p_bold_IconSize = 25.0f;
	static constexpr float h2_IconSize = 30.0f;

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

	static constexpr ImU32 selection = IM_COL32(85, 125, 255, 255);
	static constexpr ImU32 selectionInactive = IM_COL32(100, 100, 100, 255);

	static constexpr ImU32 borderColor = IM_COL32(0, 0, 0, 255);
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
	static constexpr const char* regular = "../resources/fonts/Inter_18pt-Light.ttf";
	static constexpr const char* bold = "../resources/fonts/Inter_18pt-SemiBold.ttf";
	static constexpr const char* icons = "../resources/fonts/fa-solid-900.ttf";
};