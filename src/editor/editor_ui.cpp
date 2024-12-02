#include "editor_ui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "IconsFontAwesome6.h"

#include "../src/runtime/runtime.h"
#include "../src/editor/ui_components.h"
#include "../src/editor/ui_layout.h"
#include "../src/editor/editor_window.h"
#include "../src/editor/windows/scene_window.h"
#include "../src/editor/windows/post_processing_window.h"
#include "../src/editor/windows/diagnostics_window.h"
#include "../src/utils/log.h"
#include "../src/input/cursor.h"

namespace EditorUI {

	unsigned int _idCounter = 0;
	std::vector<EditorWindow*> _windows;
	Sizing _sizing;
	Colors _colors;
	WindowFlags _windowFlags;
	Fonts _fonts;

	bool _cursorHidden = false;

	void setup()
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
		_fonts.uiRegular = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-Light.ttf", _sizing.regularFontSize);

		// Merge icons into regularFontSize font
		float iconFontSize = _sizing.iconFontSize * 2.0f / 3.0f;
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;
		_fonts.uiIcons = io.Fonts->AddFontFromFileTTF("./resources/fonts/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

		// Load other _fonts
		_fonts.uiBold = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-SemiBold.ttf", _sizing.regularFontSize);
		_fonts.uiHeadline = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-SemiBold.ttf", _sizing.headlineFontSize);
		_fonts.uiSmall = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-Light.ttf", _sizing.smallFontSize);

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
		style.TabBarOverlineSize = 0.0f;

		style.ItemSpacing = ImVec2(4.0f, 8.0f);

		ImVec4* imguiColors = style.Colors;

		imguiColors[ImGuiCol_WindowBg] = _colors.background;

		imguiColors[ImGuiCol_TitleBg] = _colors.background;
		imguiColors[ImGuiCol_TitleBgActive] = _colors.background;
		imguiColors[ImGuiCol_TitleBgCollapsed] = _colors.background;

		imguiColors[ImGuiCol_Text] = _colors.text;
		imguiColors[ImGuiCol_Button] = _colors.element;
		imguiColors[ImGuiCol_ButtonHovered] = _colors.elementHovered;
		imguiColors[ImGuiCol_ButtonActive] = _colors.elementActive;

		imguiColors[ImGuiCol_FrameBg] = _colors.element;
		imguiColors[ImGuiCol_FrameBgHovered] = _colors.elementHovered;
		imguiColors[ImGuiCol_FrameBgActive] = _colors.elementActive;

		imguiColors[ImGuiCol_SliderGrab] = _colors.elementComponent;
		imguiColors[ImGuiCol_SliderGrabActive] = _colors.elementComponent;
		imguiColors[ImGuiCol_CheckMark] = _colors.elementComponent;

		imguiColors[ImGuiCol_ResizeGrip] = _colors.element;
		imguiColors[ImGuiCol_ResizeGripActive] = _colors.elementActive;
		imguiColors[ImGuiCol_ResizeGripHovered] = _colors.elementHovered;

		imguiColors[ImGuiCol_Header] = _colors.element;
		imguiColors[ImGuiCol_HeaderHovered] = _colors.elementHovered;
		imguiColors[ImGuiCol_HeaderActive] = _colors.elementActive;

		imguiColors[ImGuiCol_SeparatorHovered] = _colors.element;
		imguiColors[ImGuiCol_SeparatorActive] = _colors.elementActive;

		imguiColors[ImGuiCol_Border] = _colors.borderColor;

		imguiColors[ImGuiCol_Tab] = _colors.tabColor;
		imguiColors[ImGuiCol_TabHovered] = _colors.tabColor;
		imguiColors[ImGuiCol_TabActive] = _colors.tabColor;
		imguiColors[ImGuiCol_TabDimmed] = _colors.tabColor;
		imguiColors[ImGuiCol_TabDimmedSelected] = _colors.tabColor;
		imguiColors[ImGuiCol_TabDimmedSelectedOverline] = _colors.elementActive;

		imguiColors[ImGuiCol_DockingPreview] = _colors.elementActive;
		imguiColors[ImGuiCol_DockingEmptyBg] = _colors.elementActive;

		ImGui_ImplGlfw_InitForOpenGL(Runtime::glfw, true);
		ImGui_ImplOpenGL3_Init("#version 460");

		SceneWindow* sceneWindow = new SceneWindow();
		_windows.push_back(sceneWindow);

		PostProcessingWindow* postProcessingWindow = new PostProcessingWindow();
		_windows.push_back(postProcessingWindow);

		DiagnosticsWindow* diagnosticsWindow = new DiagnosticsWindow();
		_windows.push_back(diagnosticsWindow);
	}

	void newFrame()
	{
		// Reset id counter for new frame
		_idCounter = 0;

		// Create new imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void render()
	{
		/* SET VARIABLES NEEDING FRAME PREPARATION */
		_cursorHidden = false;
		ImGuiIO& io = ImGui::GetIO();

		/* CREATE MAIN VIEWPORT DOCKSPACE */
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::PushFont(_fonts.uiHeadline);

		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Engine", nullptr, _windowFlags.viewport);

		ImGuiID dockspace_id = ImGui::GetID("ViewportDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();
		ImGui::PopFont();
		ImGui::PopStyleVar(3);

		/* PREPARE ALL WINDOWS */
		for (int i = 0; i < _windows.size(); i++)
		{
			_windows[i]->render();
		}

		/* SET CURSOR */
		if (_cursorHidden) {
			Cursor::setMode(CursorMode::HIDDEN);
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		}
		else {
			Cursor::setMode(CursorMode::NORMAL);
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		}

		/* RENDERING AND DRAW CALLS */
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	std::string generateId()
	{
		return ("##" + std::to_string(++_idCounter));
	}

	const Colors& getColors()
	{
		return _colors;
	}

	const WindowFlags& getWindowFlags() {
		return _windowFlags;
	}

	const Fonts& getFonts() {
		return _fonts;
	}

	void hideCursor() {
		_cursorHidden = true;
	}

	ImVec4 lighten(ImVec4 color, float amount)
	{
		float factor = 1.0f + amount;
		return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);
	}

	ImVec4 darken(ImVec4 color, float amount)
	{
		float factor = 1.0f - amount;
		return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);
	}

}