#include "editor_ui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <ImGuizmo.h>

#include "IconsFontAwesome6.h"

#include "../src/ui/ui_components.h"
#include "../src/ui/ui_layout.h"
#include "../src/ui/editor_window.h"
#include "../src/ui/windows/scene_window.h"
#include "../src/ui/windows/post_processing_window.h"
#include "../src/ui/windows/diagnostics_window.h"
#include "../src/runtime/runtime.h"

#include "../core/utils/log.h"
#include "../core/input/cursor.h"

namespace EditorUI {

	unsigned int _idCounter = 0;
	std::vector<EditorWindow*> _windows;
	Sizing _sizing;
	Colors _colors;
	WindowFlags _windowFlags;
	Fonts _fonts;

	bool _overwriteCursor = false; // Overwrites the default ui framework cursor
	int _overwriteCursorType = CursorType::DEFAULT; // Type of cursor if overwriting default cursor
	int _overwriteCursorMode = CursorMode::NORMAL; // Mode of cursor if overwriting default cursor

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
		_fonts.uiRegular = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-Light.ttf", _sizing.regularFontSize);

		// Merge icons into regular size font
		float iconFontSize = _sizing.iconFontSize * 2.0f / 3.0f;
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;
		_fonts.uiIcons = io.Fonts->AddFontFromFileTTF("../resources/fonts/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

		// Load other _fonts
		_fonts.uiBold = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-SemiBold.ttf", _sizing.regularFontSize);
		_fonts.uiHeadline = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-SemiBold.ttf", _sizing.headlineFontSize);
		_fonts.uiSmall = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-Light.ttf", _sizing.smallFontSize);

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
		// style.TabBarOverlineSize = 0.0f;

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
		ImGuizmo::BeginFrame();
	}

	void render()
	{
		/* SET VARIABLES NEEDING FRAME PREPARATION */
		_overwriteCursor = false;
		_overwriteCursorType = CursorType::DEFAULT;
		_overwriteCursorMode = CursorMode::NORMAL;

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

		/* RENDER ALL WINDOWS */
		for (int i = 0; i < _windows.size(); i++)
		{
			_windows[i]->render();
		}

		/* OVERWRITE UI FRAMEWORK CURSOR IF NEEDED */
		if (_overwriteCursor) {
			// Disable ui framework cursor management
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

			// Set cursor to overwrite type and mode
			Cursor::setType(_overwriteCursorType);
			Cursor::setMode(_overwriteCursorMode);
		}
		else {
			// Enable ui frameword cursor management
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

	void EditorUI::setCursorType(int cursorType)
	{
		// Overwrite cursor type for current frame
		_overwriteCursor = true;
		_overwriteCursorType = cursorType;
	}

	void setCursorMode(int cursorMode)
	{
		// Overwrite cursor mode for current frame
		_overwriteCursor = true;
		_overwriteCursorMode = cursorMode;
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