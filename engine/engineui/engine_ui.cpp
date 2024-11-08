#include "engine_ui.h"

std::vector<EngineWindow*> EngineUI::windows = std::vector<EngineWindow*>();

Sizing EngineUI::sizing;
Colors EngineUI::colors;
WindowFlags EngineUI::windowFlags;
Fonts EngineUI::fonts;

void EngineUI::setup() {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

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

	style.WindowPadding = ImVec2(20.0f, 20.0f);
	style.WindowRounding = 4.0f;
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.WindowBorderSize = 0.5f;

	style.FramePadding = ImVec2(3.0f, 3.0f);
	style.FrameRounding = 2.0f;

	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;

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

	imguiColors[ImGuiCol_SeparatorHovered] = colors.element;
	imguiColors[ImGuiCol_SeparatorActive] = colors.elementActive;

	imguiColors[ImGuiCol_Border] = colors.borderColor;

	ImGui_ImplGlfw_InitForOpenGL(Window::glfw, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	SceneView* sceneView = new SceneView();
	windows.push_back(sceneView);
}

void EngineUI::newFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EngineUI::render() {
	// ImGui::ShowMetricsWindow();

	for (int i = 0; i < windows.size(); i++) {
		windows[i]->prepare();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImVec4 EngineUI::lighten(ImVec4 color, float amount) {
	float factor = 1.0f + amount;
	return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);
}

ImVec4 EngineUI::darken(ImVec4 color, float amount) {
	float factor = 1.0f - amount;
	return ImVec4(color.x * factor, color.y * factor, color.z * factor, color.w);
}