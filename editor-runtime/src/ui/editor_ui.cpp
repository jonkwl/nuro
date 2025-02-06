#include "editor_ui.h"

#include <implot.h>
#include <ImGuizmo.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../core/utils/console.h"
#include "../core/input/cursor.h"
#include "../core/context/application_context.h"

#include "../src/ui/misc/ui_flex.h"
#include "../src/runtime/runtime.h"
#include "../src/ui/footer/footer.h"
#include "../src/ui/title_bar/title_bar.h"
#include "../src/ui/windows/game_window.h"
#include "../src/ui/windows/viewport_window.h"
#include "../src/ui/windows/console_window.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/windows/diagnostics_window.h"
#include "../src/ui/windows/insight_panel_window.h"
#include "../src/ui/windows/asset_browser_window.h"
#include "../src/ui/collection/IconsFontAwesome6.h"
#include "../src/ui/windows/post_processing_window.h"

namespace EditorUI {

	static const ImWchar gIconRange[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	uint32_t gIdCounter = 0;
	std::vector<EditorWindow*> gWindows;

	Fonts gFonts;

	TitleBar gTitleBar;
	Footer gFooter;

	bool gOverwriteCursor = false; // Overwrites the default imgui cursor
	int32_t gOverwriteCursorType = CursorType::DEFAULT; // Type of cursor if overwriting default cursor
	int32_t gOverwriteCursorMode = CursorMode::NORMAL; // Mode of cursor if overwriting default cursor

	uint32_t gLogoTexture = 0;

	void _mergeIcons(ImGuiIO& io, float fontSize) {
		float iconsFontSize = fontSize * 2.0f / 3.0f;

		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		iconsConfig.GlyphMinAdvanceX = iconsFontSize;

		gFonts.icons = io.Fonts->AddFontFromFileTTF(EditorFontPath::icons, iconsFontSize, &iconsConfig, gIconRange);
	}

	void setup()
	{
		//
		// CREATE IMGUI CONTEXT
		//
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImPlot::CreateContext();

		//
		// SETUP IO
		//

		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		//
		// LOAD FONTS
		//

		// p
		gFonts.p = io.Fonts->AddFontFromFileTTF(EditorFontPath::regular, EditorSizing::p_FontSize);
		_mergeIcons(io, EditorSizing::p_IconSize);
		gFonts.p_bold = io.Fonts->AddFontFromFileTTF(EditorFontPath::bold, EditorSizing::p_FontSize);
		_mergeIcons(io, EditorSizing::p_bold_IconSize);

		// h1
		gFonts.h1 = io.Fonts->AddFontFromFileTTF(EditorFontPath::regular, EditorSizing::h1_FontSize);
		gFonts.h1_bold = io.Fonts->AddFontFromFileTTF(EditorFontPath::bold, EditorSizing::h1_FontSize);

		// h2
		gFonts.h2 = io.Fonts->AddFontFromFileTTF(EditorFontPath::regular, EditorSizing::h2_FontSize);
		_mergeIcons(io, EditorSizing::h2_IconSize);
		gFonts.h2_bold = io.Fonts->AddFontFromFileTTF(EditorFontPath::bold, EditorSizing::h2_FontSize);
		_mergeIcons(io, EditorSizing::h2_bold_IconSize);

		// h3
		gFonts.h3 = io.Fonts->AddFontFromFileTTF(EditorFontPath::regular, EditorSizing::h3_FontSize);
		_mergeIcons(io, EditorSizing::h2_IconSize);
		gFonts.h3_bold = io.Fonts->AddFontFromFileTTF(EditorFontPath::bold, EditorSizing::h3_FontSize);
		_mergeIcons(io, EditorSizing::h2_IconSize);

		// h4
		gFonts.h4 = io.Fonts->AddFontFromFileTTF(EditorFontPath::regular, EditorSizing::h4_FontSize);
		_mergeIcons(io, EditorSizing::p_bold_IconSize);
		gFonts.h4_bold = io.Fonts->AddFontFromFileTTF(EditorFontPath::bold, EditorSizing::h4_FontSize);
		_mergeIcons(io, EditorSizing::p_bold_IconSize);

		// s
		gFonts.s = io.Fonts->AddFontFromFileTTF(EditorFontPath::regular, EditorSizing::s_FontSize);
		_mergeIcons(io, EditorSizing::s_IconSize);
		gFonts.s_bold = io.Fonts->AddFontFromFileTTF(EditorFontPath::bold, EditorSizing::s_FontSize);
		_mergeIcons(io, EditorSizing::s_bold_IconSize);

		//
		// STYLE COLORS
		//

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();

		style.ButtonTextAlign = ImVec2(0.0f, 0.0f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
		style.SeparatorTextAlign = ImVec2(0.0f, 0.0f);
		style.TableAngledHeadersTextAlign = ImVec2(0.0f, 0.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.0f);

		style.WindowPadding = ImVec2(EditorSizing::windowPadding, EditorSizing::windowPadding);
		style.WindowRounding = 4.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.WindowBorderSize = 0.5f;

		style.FramePadding = ImVec2(EditorSizing::framePadding, EditorSizing::framePadding);
		style.FrameRounding = 2.0f;

		style.GrabMinSize = 5.0f;
		style.GrabRounding = 0.0f;

		style.TabRounding = 1.0f;
		// style.TabBarOverlineSize = 0.0f;

		style.ItemSpacing = ImVec2(4.0f, 4.0f);

		style.PopupRounding = 10.0f;

		ImVec4* imguiColors = style.Colors;

		imguiColors[ImGuiCol_WindowBg] = ImColor(EditorColor::background);
		imguiColors[ImGuiCol_TitleBg] = ImColor(EditorColor::background);
		imguiColors[ImGuiCol_TitleBgActive] = ImColor(EditorColor::background);
		imguiColors[ImGuiCol_TitleBgCollapsed] = ImColor(EditorColor::background);

		imguiColors[ImGuiCol_Text] = ImColor(EditorColor::text);
		imguiColors[ImGuiCol_Button] = ImColor(EditorColor::element);
		imguiColors[ImGuiCol_ButtonHovered] = ImColor(EditorColor::elementHovered);
		imguiColors[ImGuiCol_ButtonActive] = ImColor(EditorColor::elementActive);

		imguiColors[ImGuiCol_FrameBg] = ImColor(EditorColor::element);
		imguiColors[ImGuiCol_FrameBgHovered] = ImColor(EditorColor::elementHovered);
		imguiColors[ImGuiCol_FrameBgActive] = ImColor(EditorColor::elementActive);

		imguiColors[ImGuiCol_SliderGrab] = ImColor(EditorColor::elementComponent);
		imguiColors[ImGuiCol_SliderGrabActive] = ImColor(EditorColor::elementComponent);
		imguiColors[ImGuiCol_CheckMark] = ImColor(EditorColor::elementComponent);

		imguiColors[ImGuiCol_ResizeGrip] = ImColor(EditorColor::element);
		imguiColors[ImGuiCol_ResizeGripActive] = ImColor(EditorColor::elementActive);
		imguiColors[ImGuiCol_ResizeGripHovered] = ImColor(EditorColor::elementHovered);

		imguiColors[ImGuiCol_Header] = ImColor(EditorColor::element);
		imguiColors[ImGuiCol_HeaderHovered] = ImColor(EditorColor::elementHovered);
		imguiColors[ImGuiCol_HeaderActive] = ImColor(EditorColor::elementActive);

		imguiColors[ImGuiCol_SeparatorHovered] = ImColor(EditorColor::element);
		imguiColors[ImGuiCol_SeparatorActive] = ImColor(EditorColor::elementActive);

		imguiColors[ImGuiCol_Border] = ImColor(EditorColor::borderColor);

		imguiColors[ImGuiCol_Tab] = ImColor(EditorColor::tabColor);
		imguiColors[ImGuiCol_TabHovered] = ImColor(EditorColor::tabColor);
		imguiColors[ImGuiCol_TabActive] = ImColor(EditorColor::tabColor);
		imguiColors[ImGuiCol_TabDimmed] = ImColor(EditorColor::tabColor);
		imguiColors[ImGuiCol_TabDimmedSelected] = ImColor(EditorColor::tabColor);
		imguiColors[ImGuiCol_TabDimmedSelectedOverline] = ImColor(EditorColor::elementActive);

		imguiColors[ImGuiCol_DockingPreview] = ImColor(EditorColor::elementActive);
		imguiColors[ImGuiCol_DockingEmptyBg] = ImColor(EditorColor::elementActive);

		//
		// IMPLEMENT IMGUI BACKEND
		//

		switch (ApplicationContext::readConfiguration().api) {
		case API::OPENGL:
			ImGui_ImplGlfw_InitForOpenGL(ApplicationContext::getWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 460");
			break;
		default:
			Console::out::error("Editor UI", "Couldn't setup editor ui, backend api is invalid");
			break;
		}

		//
		// CREATE EDITOR WINDOWS
		// Note: Permanently allocated, as they will be needed throughout the whole runtime
		//

		ViewportWindow* viewportWindow = new ViewportWindow();
		gWindows.emplace_back(viewportWindow);

		GameWindow* gameWindow = new GameWindow();
		gWindows.emplace_back(gameWindow);

		PostProcessingWindow* postProcessingWindow = new PostProcessingWindow(Runtime::getGameViewPipeline().getProfile());
		gWindows.emplace_back(postProcessingWindow);

		DiagnosticsWindow* diagnosticsWindow = new DiagnosticsWindow();
		gWindows.emplace_back(diagnosticsWindow);

		ConsoleWindow* consoleWindow = new ConsoleWindow();
		gWindows.emplace_back(consoleWindow);

		RegistryWindow* registryWindow = new RegistryWindow();
		gWindows.emplace_back(registryWindow);

		InsightPanelWindow* insightPanelWindow = new InsightPanelWindow();
		gWindows.emplace_back(insightPanelWindow);

		AssetBrowserWindow* assetBrowserWindow = new AssetBrowserWindow();
		gWindows.emplace_back(assetBrowserWindow);

		//
		// SETUP WINDOW ELEMENTS
		//

		TitleBarStyle& titleBarStyle = gTitleBar.getStyle();
		titleBarStyle.primaryFont = gFonts.h4_bold;
		titleBarStyle.secondaryFont = gFonts.p;
		titleBarStyle.workspaceBarFont = gFonts.h4;

		FooterStyle& footerStyle = gFooter.getStyle();
		footerStyle.primaryFont = gFonts.p_bold;
		footerStyle.secondaryFont = gFonts.p;
	}

	void newFrame()
	{
		// Reset id counter for new frame
		gIdCounter = 0;

		// Create new imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void render()
	{
		//
		// SET VARIABLES NEEDING FRAME PREPARATION
		//
		
		gOverwriteCursor = false;
		gOverwriteCursorType = CursorType::DEFAULT;
		gOverwriteCursorMode = CursorMode::NORMAL;

		ImGuiIO& io = ImGui::GetIO();

		//
		// CREATE MAIN VIEWPORT DOCKSPACE
		//

		ImGuiViewport& viewport = *ImGui::GetMainViewport();

		// Push font for tabs
		ImGui::PushFont(gFonts.h4);

		//
		// RENDER WINDOW ELEMENTS
		//

		float titleBarHeight = gTitleBar.getStyle().height;
		float footerHeight = gFooter.getStyle().height;

		gTitleBar.render(viewport);
		gFooter.render(viewport);

		//
		// CREATE APPLICATION MAIN DOCKSPACE
		//

		ImGui::SetNextWindowPos(ImVec2(viewport.Pos.x, viewport.Pos.y + titleBarHeight));
		ImGui::SetNextWindowSize(ImVec2(viewport.Size.x, viewport.Size.y - titleBarHeight - footerHeight));
		ImGui::SetNextWindowViewport(viewport.ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Engine", nullptr, EditorFlag::viewport);

		ImGuiID dockspace_id = ImGui::GetID("ViewportDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();
		ImGui::PopFont();
		ImGui::PopStyleVar(3);

		//
		// RENDER ALL WINDOWS
		//

		for (int32_t i = 0; i < gWindows.size(); i++)
		{
			gWindows[i]->render();
		}

		//
		// HANDLE CUSTOM CURSOR
		//

		if (gOverwriteCursor) {
			// Disable imgui cursor management
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

			// Set cursor to overwrite type and mode
			Cursor::setType(gOverwriteCursorType);
			Cursor::setMode(gOverwriteCursorMode);
		}
		else {
			// Enable ui frameword cursor management
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		}

		//
		// DRAW CALLS
		//
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	ImGuiID generateId()
	{
		return ++gIdCounter;
	}

	std::string generateIdString()
	{
		return "##" + std::to_string(++gIdCounter);
	}

	const Fonts& getFonts() {
		return gFonts;
	}

	void EditorUI::setCursorType(int32_t cursorType)
	{
		// Overwrite cursor type for current frame
		gOverwriteCursor = true;
		gOverwriteCursorType = cursorType;
	}

	void setCursorMode(int32_t cursorMode)
	{
		// Overwrite cursor mode for current frame
		gOverwriteCursor = true;
		gOverwriteCursorMode = cursorMode;
	}

}