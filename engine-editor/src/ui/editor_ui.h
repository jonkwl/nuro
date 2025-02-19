#pragma once

#include <imgui.h>
#include <vector>
#include <string>

#include <iostream>
#include <cstdlib>

#define EDITOR_ASSERT(condition, message) \
  if (!(condition)) { \
    std::cerr << "Assertion failed: " << message << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    std::abort(); \
  }

namespace EditorUI
{
	struct Fonts
	{
		// Paragraph
		ImFont* p = nullptr;
		ImFont* p_bold = nullptr;

		// Headline 1
		ImFont* h1 = nullptr;
		ImFont* h1_bold = nullptr;

		// Headline 2
		ImFont* h2 = nullptr;
		ImFont* h2_bold = nullptr;

		// Headline 3
		ImFont* h3 = nullptr;
		ImFont* h3_bold = nullptr;

		// Headline 4
		ImFont* h4 = nullptr;
		ImFont* h4_bold = nullptr;
		
		// Small
		ImFont* s = nullptr;
		ImFont* s_bold = nullptr;

		// Icons
		ImFont* icons = nullptr;
	};

	// Sets up editor ui
	void setup();

	// Begins a new frame for the editor ui
	void newFrame();

	// Renders the ui frame
	void render();

	// Generates a unique imgui id
	ImGuiID generateId();

	// Generates a unique imgui id as a string
	std::string generateIdString();

	// Returns all loded fonts
	const Fonts& getFonts();

	// Sets the current editor ui cursor type
	void setCursorType(int32_t cursorType);

	// Sets the current editor ui cursor mode
	void setCursorMode(int32_t cursorMode);
};

struct EditorSizing
{
	static constexpr float p_FontSize = 14.0f;
	static constexpr float h1_FontSize = 26.0f;
	static constexpr float h2_FontSize = 22.0f;
	static constexpr float h3_FontSize = 17.0f;
	static constexpr float h4_FontSize = 15.0f;
	static constexpr float s_FontSize = 13.0f;

	static constexpr float p_IconSize = 22.0f;
	static constexpr float p_bold_IconSize = 25.0f;

	static constexpr float h2_IconSize = 30.0f;
	static constexpr float h2_bold_IconSize = 30.0f;

	static constexpr float s_IconSize = 16.0f;
	static constexpr float s_bold_IconSize = 16.0f;

	static constexpr float windowPadding = 30.0f;
	static constexpr float framePadding = 3.0f;
};

struct EditorColor
{
	static constexpr ImU32 background = IM_COL32(16, 16, 16, 255);

	static constexpr ImU32 text = IM_COL32(255, 255, 255, 255);
	static constexpr ImU32 text_transparent = IM_COL32(255, 255, 255, 210);

	static constexpr ImU32 element = IM_COL32(51, 51, 51, 255);
	static constexpr ImU32 elementHovered = IM_COL32(64, 64, 64, 255);
	static constexpr ImU32 elementActive = IM_COL32(77, 77, 77, 255);
	static constexpr ImU32 elementComponent = IM_COL32(178, 178, 178, 255);

	static constexpr ImU32 element_transparent = IM_COL32(80, 80, 80, 130);
	static constexpr ImU32 elementHovered_transparentOverlay = IM_COL32(255, 255, 255, 10);
	static constexpr ImU32 elementActive_transparentOverlay = IM_COL32(255, 255, 255, 35);

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
	static constexpr const char* icons = "../resources/fonts/fa-solid-900-free.ttf";
	static constexpr const char* iconsFull = "../resources/fonts/iconpack-full.ttf";
};