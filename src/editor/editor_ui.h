#pragma once

#include <imgui.h>
#include <vector>
#include <string>

class EditorWindow;

struct Sizing
{
	float regularFontSize = 14.0f;
	float iconFontSize = 22.0f;
	float headlineFontSize = 15.0f;
};

struct Colors
{
	ImVec4 background = ImVec4(0.08f, 0.08f, 0.08f, 0.95f);
	ImVec4 text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImVec4 element = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 elementHovered = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	ImVec4 elementActive = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	ImVec4 elementComponent = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

	ImVec4 borderColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	ImVec4 selection = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);

	ImVec4 tabColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
};

struct WindowFlags
{
	ImGuiWindowFlags viewport = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
	ImGuiWindowFlags standard = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	ImGuiWindowFlags fixed = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
};

struct Fonts
{
	ImFont* uiRegular;
	ImFont* uiBold;
	ImFont* uiHeadline;
	ImFont* uiIcons;
};

class EditorUI
{
public:
	static void setup();
	static void newFrame();
	static void render();

	static ImVec4 lighten(ImVec4 color, float amount);
	static ImVec4 darken(ImVec4 color, float amount);

	static std::vector<EditorWindow*> windows;

	static Sizing sizing;
	static Colors colors;
	static WindowFlags windowFlags;
	static Fonts fonts;

	static std::string getId();

private:
	static unsigned int idCounter;
};