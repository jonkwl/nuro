#include "engine_ui.h"

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

	fonts.uiRegular = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-Light.ttf", 14);
	fonts.uiBold = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter_18pt-SemiBold.ttf", 14);

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowPadding = ImVec2(16.0f, 16.0f);
	style.WindowRounding = 4.0f;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	style.FramePadding = ImVec2(2.0f, 2.0f);
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

	ImGui_ImplGlfw_InitForOpenGL(Window::glfw, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void EngineUI::newFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EngineUI::render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}