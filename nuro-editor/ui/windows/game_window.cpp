#include "game_window.h"

#include <rendering/postprocessing/post_processing.h>
#include <input/input.h>
#include <input/cursor.h>
#include <time/time.h>
#include <utils/console.h>
#include <rendering/transformation/transformation.h>
#include <rendering/shadows/shadow_map.h>

GameWindow::GameWindow() : currentContentAvail(ImVec2(0.0f, 0.0f)),
lastContentAvail(ImVec2(0.0f, 0.0f)),
lastWindowSize(ImVec2(0.0f, 0.0f)),
windowFocused(false),
windowHovered(false),
gameViewBounds(ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
{
	// Setup no camera indicator
	noCameraIndicator.color = IM_COL32(50, 35, 0, 255);
	noCameraIndicator.padding = ImVec2(45.0f, 30.0f);
	noCameraIndicator.rounding = 25.0f;
	noCameraIndicator.foreground = true;
	noCameraIndicator.outline = true;
	noCameraIndicator.outlineStrength = 1.0f;
	noCameraIndicator.outlineColor = IM_COL32(255, 180, 0, 125);
	noCameraIndicator.smoothing = false;
	DynamicText noCameraText(EditorUI::getFonts().h3);
	std::string noCameraIcon = ICON_FA_EYE_SLASH;
	noCameraText.text = noCameraIcon + "     No camera available, can't render";
	noCameraText.color = IM_COL32(255, 255, 255, 255);
	noCameraText.alignment = TextAlign::CENTER;
	noCameraIndicator.addText(noCameraText);
}

void GameWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string title = UIUtils::windowTitle("Game View");
	bool windowVisible = ImGui::Begin(title.c_str(), nullptr, EditorFlag::standard);
	{

		// Get draw list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		// Get initial cursor position 
		ImVec2 position = ImGui::GetCursorScreenPos();

		windowFocused = UIUtils::windowFocused();
		windowHovered = UIUtils::windowHovered();

		// IMComponents::headline("Game View", ICON_FA_BOLT_LIGHTNING, HeadlineJustification::CENTER, false);

		// Get game window size
		currentContentAvail = ImGui::GetContentRegionAvail();

		// Render game view
		renderGameView();

		// Render game windows toolbar
		renderToolbar(drawList, position);

		// Cache window position and size
		lastWindowPosition = ImGui::GetWindowPos();
		lastWindowSize = ImGui::GetWindowSize();

	}
	ImGui::End();
	ImGui::PopStyleVar();

	if (windowVisible) renderForegroundIndicators();
}

void GameWindow::renderToolbar(ImDrawList& drawList, ImVec2 position)
{
	//
}

void GameWindow::renderGameView()
{
	// Prepare game view data
	GameViewPipeline& pipeline = Runtime::getGameViewPipeline();
	bool availableCamera = pipeline.getCameraAvailable();
	uint32_t output = pipeline.getOutput();

	// If no camera is available
	if (!availableCamera) {
		output = 0;
	}

	// Check if window is currently being resized
	bool currentlyResizing = currentContentAvail != lastContentAvail;
	if (currentlyResizing) output = 0;

	// Draw black background
	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail(), IM_COL32(0, 0, 0, 255));

	// float aspectRatio = 16.0f / 9.0f;
	float aspectRatio = 0.0f;
	ImVec2 size, offset;
	if (aspectRatio) {
		UIUtils::calculateAspectFitting(aspectRatio, size, offset);
		ImGui::SetCursorPos(offset);
	}
	else {
		size = ImGui::GetContentRegionAvail();
	}

	// Render target
	ImGui::Image(output, size, ImVec2(0, 1), ImVec2(1, 0));

	ImVec2 boundsMin = ImGui::GetItemRectMin();
	ImVec2 boundsMax = ImGui::GetItemRectMax();
	gameViewBounds = ImVec4(boundsMin.x, boundsMin.y, boundsMax.x, boundsMax.y);

	// UIUtils::keepCursorInBounds(sceneViewBounds, positionedCursor);

	// Check if game window has been resized
	if (currentlyResizing && !Input::mouseDown(MouseButton::LEFT)) {
		pipeline.resizeViewport(size.x, size.y);
		lastContentAvail = currentContentAvail;
	}
}

void GameWindow::renderForegroundIndicators()
{
	GameViewPipeline& pipeline = Runtime::getGameViewPipeline();

	// Render no camera indicator if no camera is available
	if (!pipeline.getCameraAvailable()) {
		ImVec2 size = noCameraIndicator.getSize();
		noCameraIndicator.position = ImVec2(
			lastWindowPosition.x + lastWindowSize.x * 0.5f - size.x * 0.5f,
			lastWindowPosition.y + lastWindowSize.y * 0.5f - size.y * 0.5f
		);
		noCameraIndicator.draw();
	}
}