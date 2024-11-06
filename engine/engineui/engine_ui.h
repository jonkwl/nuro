#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "../engine/window/window.h"

#include <iostream>

struct Colors {
	ImVec4 background = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	ImVec4 text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImVec4 element = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 elementHovered = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	ImVec4 elementActive = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	ImVec4 elementComponent = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
};

struct WindowFlags {
	ImGuiWindowFlags fixed = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
};

struct Fonts {
	ImFont* uiRegular;
	ImFont* uiBold;
};

class EngineUI
{
public:
	static void setup();
	static void newFrame();
	static void render();

	static Colors colors;
	static WindowFlags windowFlags;
	static Fonts fonts;
};