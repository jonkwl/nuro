#pragma once

#include <imgui.h>
#include <vector>
#include <string>

#include "../core/context/application_context.h"

namespace EditorUI
{
	struct Fonts
	{
		ImFont* uiRegular;
		ImFont* uiBold;
		ImFont* uiHeadline;
		ImFont* uiSmall;
		ImFont* uiIcons;
	};

	void setup(ApplicationContext* context);
	void newFrame();
	void render();

	std::string generateId();

	const Fonts& getFonts();

	void setCursorType(int32_t cursorType);
	void setCursorMode(int32_t cursorMode);
};

struct EditorSizing
{
	static constexpr float regularFontSize = 14.0f;
	static constexpr float iconFontSize = 22.0f;
	static constexpr float headlineFontSize = 15.0f;
	static constexpr float smallFontSize = 12.0f;
};

struct EditorColors
{
	static constexpr ImU32 background = IM_COL32(20, 20, 20, 242);
	static constexpr ImU32 text = IM_COL32(255, 255, 255, 255);

	static constexpr ImU32 element = IM_COL32(51, 51, 51, 255);
	static constexpr ImU32 elementHovered = IM_COL32(64, 64, 64, 255);
	static constexpr ImU32 elementActive = IM_COL32(77, 77, 77, 255);
	static constexpr ImU32 elementComponent = IM_COL32(178, 178, 178, 255);

	static constexpr ImU32 borderColor = IM_COL32(0, 0, 0, 255);
	static constexpr ImU32 selection = IM_COL32(0, 0, 255, 255);
	static constexpr ImU32 tabColor = IM_COL32(38, 38, 38, 255);
};

struct EditorFlags
{
	static constexpr ImGuiWindowFlags viewport = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
	static constexpr ImGuiWindowFlags standard = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	static constexpr ImGuiWindowFlags fixed = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
};