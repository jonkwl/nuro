#include "game_window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/type_ptr.hpp>
#include <gtx/matrix_decompose.hpp>

#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/input/input.h"
#include "../core/input/cursor.h"
#include "../core/time/time.h"
#include "../core/utils/log.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/shadows/shadow_map.h"

GameWindow::GameWindow() : currentWindowSize(glm::vec2(0.0f)),
lastWindowSize(glm::vec2(0.0f)),
windowFocused(false),
windowHovered(false),
gameViewBounds(glm::vec4(0.0f))
{
}

void GameWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("  Game  ", nullptr, EditorUI::getWindowFlags().standard);
	{

		windowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

		UIComponents::space(0.0f, 14.0f);
		UIComponents::headline("Game", ICON_FA_PLAY, HeadlineJustification::CENTER);

		// Get game window size
		currentWindowSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

		// Render game windows toolbar
		renderToolbar();

		// Render game view
		renderGameView();

	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void GameWindow::renderToolbar()
{
	bool tmp = false;

	// Render toggle buttons for render options
	UILayout::beginFlex("toggles", FlexType::ROW, UILayout::FULL_WIDTH, 40.0f, Justification::CENTER, Alignment::CENTER, 1.0f);
	{
		UIComponents::toggleButton(ICON_FA_PLAY, tmp, "Play");
		UIComponents::toggleButton(ICON_FA_PAUSE, tmp, "Pause");
		UIComponents::toggleButton(ICON_FA_FORWARD_STEP, tmp, "Step");
	}
	UILayout::endFlex();

	// Spacing
	UIComponents::space(0.0f, 5.0f);

	// Render indicators for game view setup
	/*UILayout::beginFlex("setup", FlexType::ROW, UILayout::FULL_WIDTH, 12.0f, Justification::START, Alignment::CENTER, 1.0f);
	{
		UIComponents::space(1.0f, 0.0f);
		UIComponents::tryIcon(ICON_FA_GAUGE);
		UIComponents::label("FPS: " + 0);
	}
	UILayout::endFlex();*/
}

void GameWindow::renderGameView()
{
	// Prepare game view data
	GameViewPipeline& pipeline = Runtime::getGameViewPipeline();
	bool availableCamera = pipeline.getCameraAvailable();
	uint32_t output = pipeline.getOutput();

	// Warn if camera isn't available
	if (!pipeline.getCameraAvailable()) {
		ImVec4 color = ImVec4(0.94f, 0.72f, 0.29f, 1.0f);
		ImGui::SetCursorPosX(25.0f);
		UIComponents::tryIcon(ICON_FA_TRIANGLE_EXCLAMATION, color, 1.0f);
		ImGui::SetCursorPosX(45.0f);
		UIComponents::labelBold("Warning: No camera found, can't render", color);
		output = 0;
	}

	// Render game view
	ImGui::BeginChild("GameView", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		// Check if window is currently being resized
		bool currentlyResizing = currentWindowSize != lastWindowSize;
		if (currentlyResizing) output = 0;

		float aspectRatio = 16.0f / 9.0f;
		ImVec2 size, offset;
		UIUtils::calculateAspectFitting(aspectRatio, size, offset);

		ImGui::SetCursorPos(offset);

		// Render target
		ImGui::Image(output, size, ImVec2(0, 1), ImVec2(1, 0));

		ImVec2 boundsMin = ImGui::GetItemRectMin();
		ImVec2 boundsMax = ImGui::GetItemRectMax();
		gameViewBounds = glm::vec4(boundsMin.x, boundsMin.y, boundsMax.x, boundsMax.y);

		// UIUtils::keepCursorInBounds(sceneViewBounds, positionedCursor);

		// Check if game window has been resized
		if (currentlyResizing && !Input::mouseDown(MouseButton::LEFT)) {
			pipeline.resizeViewport(size.x, size.y);
			lastWindowSize = currentWindowSize;
		}
	}
	ImGui::EndChild();
}