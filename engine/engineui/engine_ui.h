#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "../engine/window/window.h"
#include "../engine/engineui/engine_window.h"
#include "../engine/engineui/windows/scene_view.h"

#include "IconsFontAwesome6.h"

#include <vector>

struct Sizing {
	float regularFontSize = 14.0f;
	float iconFontSize = 24.0f;
	float headlineFontSize = 15.0f;
};

struct Colors {
	ImVec4 background = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
	ImVec4 text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImVec4 element = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 elementHovered = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	ImVec4 elementActive = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	ImVec4 elementComponent = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

	ImVec4 borderColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct WindowFlags {
	ImGuiWindowFlags fixed = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
};

struct Fonts {
	ImFont* uiRegular;
	ImFont* uiBold;
	ImFont* uiHeadline;
	ImFont* uiIcons;
};

class EngineUI
{
public:
	static void setup();
	static void newFrame();
	static void render();

	static void spacing();
	static void headline(std::string title);

	static ImVec4 lighten(ImVec4 color, float amount);
	static ImVec4 darken(ImVec4 color, float amount);

	static std::vector<EngineWindow*> windows;

	static Sizing sizing;
	static Colors colors;
	static WindowFlags windowFlags;
	static Fonts fonts;
};