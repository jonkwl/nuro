#include "editor_ui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>
#include <ImGuizmo.h>

#include "IconsFontAwesome6.h"

#include "../src/ui/ui_components.h"
#include "../src/ui/ui_layout.h"
#include "../src/ui/editor_window.h"
#include "../src/runtime/runtime.h"

#include "../core/utils/log.h"
#include "../core/input/cursor.h"

#include "../src/ui/windows/scene_window.h"
#include "../src/ui/windows/game_window.h"
#include "../src/ui/windows/post_processing_window.h"
#include "../src/ui/windows/diagnostics_window.h"
#include "../src/ui/windows/console_window.h"
#include "../src/ui/windows/hierarchy_window.h"

namespace EditorUI {

	uint32_t _idCounter = 0;
	std::vector<EditorWindow*> _windows;

	Fonts _fonts;

	bool _overwriteCursor = false; // Overwrites the default imgui cursor
	int32_t _overwriteCursorType = CursorType::DEFAULT; // Type of cursor if overwriting default cursor
	int32_t _overwriteCursorMode = CursorMode::NORMAL; // Mode of cursor if overwriting default cursor

	void setup(ApplicationContext* context)
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
		_fonts.uiRegular = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-Light.ttf", EditorSizing::regularFontSize);

		// Merge icons into regular size font
		float iconFontSize = EditorSizing::iconFontSize * 2.0f / 3.0f;
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;
		_fonts.uiIcons = io.Fonts->AddFontFromFileTTF("../resources/fonts/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

		// Load other _fonts
		_fonts.uiBold = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-SemiBold.ttf", EditorSizing::regularFontSize);
		_fonts.uiHeadline = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-SemiBold.ttf", EditorSizing::headlineFontSize);
		_fonts.uiSmall = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter_18pt-Light.ttf", EditorSizing::smallFontSize);

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

		imguiColors[ImGuiCol_WindowBg] = ImColor(EditorColors::background);
		imguiColors[ImGuiCol_TitleBg] = ImColor(EditorColors::background);
		imguiColors[ImGuiCol_TitleBgActive] = ImColor(EditorColors::background);
		imguiColors[ImGuiCol_TitleBgCollapsed] = ImColor(EditorColors::background);

		imguiColors[ImGuiCol_Text] = ImColor(EditorColors::text);
		imguiColors[ImGuiCol_Button] = ImColor(EditorColors::element);
		imguiColors[ImGuiCol_ButtonHovered] = ImColor(EditorColors::elementHovered);
		imguiColors[ImGuiCol_ButtonActive] = ImColor(EditorColors::elementActive);

		imguiColors[ImGuiCol_FrameBg] = ImColor(EditorColors::element);
		imguiColors[ImGuiCol_FrameBgHovered] = ImColor(EditorColors::elementHovered);
		imguiColors[ImGuiCol_FrameBgActive] = ImColor(EditorColors::elementActive);

		imguiColors[ImGuiCol_SliderGrab] = ImColor(EditorColors::elementComponent);
		imguiColors[ImGuiCol_SliderGrabActive] = ImColor(EditorColors::elementComponent);
		imguiColors[ImGuiCol_CheckMark] = ImColor(EditorColors::elementComponent);

		imguiColors[ImGuiCol_ResizeGrip] = ImColor(EditorColors::element);
		imguiColors[ImGuiCol_ResizeGripActive] = ImColor(EditorColors::elementActive);
		imguiColors[ImGuiCol_ResizeGripHovered] = ImColor(EditorColors::elementHovered);

		imguiColors[ImGuiCol_Header] = ImColor(EditorColors::element);
		imguiColors[ImGuiCol_HeaderHovered] = ImColor(EditorColors::elementHovered);
		imguiColors[ImGuiCol_HeaderActive] = ImColor(EditorColors::elementActive);

		imguiColors[ImGuiCol_SeparatorHovered] = ImColor(EditorColors::element);
		imguiColors[ImGuiCol_SeparatorActive] = ImColor(EditorColors::elementActive);

		imguiColors[ImGuiCol_Border] = ImColor(EditorColors::borderColor);

		imguiColors[ImGuiCol_Tab] = ImColor(EditorColors::tabColor);
		imguiColors[ImGuiCol_TabHovered] = ImColor(EditorColors::tabColor);
		imguiColors[ImGuiCol_TabActive] = ImColor(EditorColors::tabColor);
		imguiColors[ImGuiCol_TabDimmed] = ImColor(EditorColors::tabColor);
		imguiColors[ImGuiCol_TabDimmedSelected] = ImColor(EditorColors::tabColor);
		imguiColors[ImGuiCol_TabDimmedSelectedOverline] = ImColor(EditorColors::elementActive);

		imguiColors[ImGuiCol_DockingPreview] = ImColor(EditorColors::elementActive);
		imguiColors[ImGuiCol_DockingEmptyBg] = ImColor(EditorColors::elementActive);

		// Initialize imgui backend
		switch (context->readConfiguration().api) {
		case API::OPENGL:
			ImGui_ImplGlfw_InitForOpenGL(context->getWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 460");
			break;
		default:
			Log::printError("Editor UI", "Couldn't setup editor ui, backend api is invalid");
			break;
		}

		// Create editor windows
		SceneWindow* sceneWindow = new SceneWindow();
		_windows.push_back(sceneWindow);

		GameWindow* gameWindow = new GameWindow();
		_windows.push_back(gameWindow);

		PostProcessingWindow* postProcessingWindow = new PostProcessingWindow(Runtime::getGameViewPipeline().getProfile());
		_windows.push_back(postProcessingWindow);

		DiagnosticsWindow* diagnosticsWindow = new DiagnosticsWindow();
		_windows.push_back(diagnosticsWindow);

		ConsoleWindow* consoleWindow = new ConsoleWindow();
		_windows.push_back(consoleWindow);

		HierarchyWindow* hierarchyWindow = new HierarchyWindow();
		_windows.push_back(hierarchyWindow);
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
		ImGui::Begin("Engine", nullptr, EditorFlags::viewport);

		ImGuiID dockspace_id = ImGui::GetID("ViewportDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();
		ImGui::PopFont();
		ImGui::PopStyleVar(3);

		/* RENDER ALL WINDOWS */
		for (int32_t i = 0; i < _windows.size(); i++)
		{
			_windows[i]->render();
		}

		/* OVERWRITE IMGUI CURSOR IF NEEDED */
		if (_overwriteCursor) {
			// Disable imgui cursor management
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

	const Fonts& getFonts() {
		return _fonts;
	}

	void EditorUI::setCursorType(int32_t cursorType)
	{
		// Overwrite cursor type for current frame
		_overwriteCursor = true;
		_overwriteCursorType = cursorType;
	}

	void setCursorMode(int32_t cursorMode)
	{
		// Overwrite cursor mode for current frame
		_overwriteCursor = true;
		_overwriteCursorMode = cursorMode;
	}

}