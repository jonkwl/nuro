#include "engine_ui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "IconsFontAwesome6.h"

#include "../src/runtime/runtime.h"
#include "../src/editor/editor_window.h"
#include "../src/editor/ui_components.h"
#include "../src/editor/ui_layout.h"
#include "../src/editor/windows/scene_window.h"
#include "../src/editor/windows/post_processing_window.h"
#include "../src/editor/windows/diagnostics_window.h"

std::vector<EditorWindow*> EditorUI::windows = std::vector<EditorWindow*>();

Sizing EditorUI::sizing;
Colors EditorUI::colors;
WindowFlags EditorUI::windowFlags;
Fonts EditorUI::fonts;

unsigned int EditorUI::idCounter = 0;

void EditorUI::setup()
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Load default font
	fonts.uiRegular = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-Light.ttf", sizing.regularFontSize);

	// Merge icons into regularFontSize font
	float iconFontSize = sizing.iconFontSize * 2.0f / 3.0f;
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	fonts.uiIcons = io.Fonts->AddFontFromFileTTF("./resources/fonts/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

	// Load other fonts
	fonts.uiBold = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-SemiBold.ttf", sizing.regularFontSize);
	fonts.uiHeadline = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-SemiBold.ttf", sizing.headlineFontSize);

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.ButtonTextAlign = ImVec2(0.0f, 0.0f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	style.SeparatorTextAlign = ImVec2(0.0f, 0.0f);
	style.TableAngledHeadersTextAlign = ImVec2(0.0f, 0.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.0f);

	style.WindowPadding = ImVec2(30.0f, 30.0f);
	style.WindowRounding = 4.0f;
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.WindowBorderSize = 0.5f;

	style.FramePadding = ImVec2(3.0f, 3.0f);
	style.FrameRounding = 2.0f;

	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;

	style.TabRounding = 1.0f;

	style.ItemSpacing = ImVec2(4.0f, 8.0f);

	ImVec4* imguiColors = style.Colors;

	imguiColors[ImGuiCol_WindowBg] = colors.background;

	imguiColors[ImGuiCol_TitleBg] = colors.background;
	imguiColors[ImGuiCol_TitleBgActive] = colors.background;
	imguiColors[ImGuiCol_TitleBgCollapsed] = colors.background;

	imguiColors[ImGuiCol_Text] = colors.text;
	imguiColors[ImGuiCol_Button] = colors.element;
	imguiColors[ImGuiCol_ButtonHovered] = colors.elementHovered;
	imguiColors[ImGuiCol_ButtonActive] = colors.elementActive;

	imguiColors[ImGuiCol_FrameBg] = colors.element;
	imguiColors[ImGuiCol_FrameBgHovered] = colors.elementHovered;
	imguiColors[ImGuiCol_FrameBgActive] = colors.elementActive;

	imguiColors[ImGuiCol_SliderGrab] = colors.elementComponent;
	imguiColors[ImGuiCol_SliderGrabActive] = colors.elementComponent;
	imguiColors[ImGuiCol_CheckMark] = colors.elementComponent;

	imguiColors[ImGuiCol_ResizeGrip] = colors.element;
	imguiColors[ImGuiCol_ResizeGripActive] = colors.elementActive;
	imguiColors[ImGuiCol_ResizeGripHovered] = colors.elementHovered;

	imguiColors[ImGuiCol_Header] = colors.element;
	imguiColors[ImGuiCol_HeaderHovered] = colors.elementHovered;
	imguiColors[ImGuiCol_HeaderActive] = colors.elementActive;

	imguiColors[ImGuiCol_SeparatorHovered] = colors.element;
	imguiColors[ImGuiCol_SeparatorActive] = colors.elementActive;

	imguiColors[ImGuiCol_Border] = colors.borderColor;

	imguiColors[ImGuiCol_Tab] = colors.elementActive;
	imguiColors[ImGuiCol_TabHovered] = colors.elementActive;
	imguiColors[ImGuiCol_TabActive] = colors.elementActive;

	imguiColors[ImGuiCol_TabDimmed] = colors.elementActive;
	imguiColors[ImGuiCol_TabDimmedSelected] = colors.elementActive;
	imguiColors[ImGuiCol_TabDimmedSelectedOverline] = colors.elementActive;

	imguiColors[ImGuiCol_DockingPreview] = colors.elementActive;
	imguiColors[ImGuiCol_DockingEmptyBg] = colors.elementActive;

	ImGui_ImplGlfw_InitForOpenGL(Runtime::glfw, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	SceneWindow* sceneWindow = new SceneWindow();
	windows.push_back(sceneWindow);

	PostProcessingWindow* postProcessingWindow = new PostProcessingWindow();
	windows.push_back(postProcessingWindow);

	DiagnosticsWindow* diagnosticsWindow = new DiagnosticsWindow();
	windows.push_back(diagnosticsWindow);
}

void EditorUI::newFrame()
{
	// Reset id counter for new frame
	idCounter = 0;

	// Create new imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EditorUI::render()
{

	/* CREATE MAIN VIEWPORT DOCKSPACE */
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::PushFont(fonts.uiHeadline);

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Engine", nullptr, windowFlags.viewport);

	ImGuiID dockspace_id = ImGui::GetID("ViewportDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleVar(3);

	/* PREPARE ALL WINDOWS */
	for (int i = 0; i < windows.size(); i++)
	{
		if (Runtime::inspectorMode && Runtime::showEngineUI)
		{
			windows[i]->prepare();
		}
	}

	/* RENDERING AND DRAW CALLS */
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImVec4 EditorUI::lighten(ImVec4 color, float amount)
{
	float factor = 1.0f + amount;
	return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);
}

ImVec4 EditorUI::darken(ImVec4 color, float amount)
{
	float factor = 1.0f - amount;
	return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);
}

std::string EditorUI::getId()
{
	return ("##" + std::to_string(++idCounter));
}