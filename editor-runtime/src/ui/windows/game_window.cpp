#include "game_window.h"

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
	// Setup no camera indicator
	noCameraIndicator.color = IM_COL32(50, 35, 0, 255);
	noCameraIndicator.padding = ImVec2(45.0f, 30.0f);
	noCameraIndicator.rounding = 25.0f;
	noCameraIndicator.foreground = true;
	noCameraIndicator.outline = true;
	noCameraIndicator.outlineStrength = 1.0f;
	noCameraIndicator.outlineColor = IM_COL32(255, 180, 0, 125);
	noCameraIndicator.horizontalAlignment = Horizontal::CENTER;
	noCameraIndicator.verticalAlignment = Vertical::CENTER;
	noCameraIndicator.smoothing = false;
	UIText noCameraText(EditorUI::getFonts().uiBig);
	std::string noCameraIcon = ICON_FA_EYE_SLASH;
	noCameraText.text = noCameraIcon + "     No camera found, can't render";
	noCameraText.color = IM_COL32(255, 255, 255, 255);
	noCameraText.alignment = TextAlign::CENTER;
	noCameraIndicator.addText(noCameraText);
}

void GameWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("  Game  ", nullptr, EditorFlag::standard);
	{

		windowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

		UIComponents::space(0.0f, 14.0f);
		UIComponents::headline("Game", ICON_FA_BOLT_LIGHTNING, HeadlineJustification::CENTER);

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

	bool gameRunning = Runtime::gameRunning();
	bool gamePaused = Runtime::gamePaused();

	// Render toggle buttons for render options
	UIFlex::beginFlex("toggles", FlexType::ROW, UIFlex::FULL_WIDTH, 40.0f, Justification::CENTER, Alignment::CENTER, 1.0f);
	{
		if (gameRunning) {
			if (gamePaused) {
				if (UIComponents::buttonBig(ICON_FA_PLAY, IM_COL32(255, 165, 50, 255), "Continue")) {
					Runtime::continueGame();
				}
			}
			else {
				if (UIComponents::buttonBig(ICON_FA_PAUSE, "Pause")) {
					Runtime::pauseGame();
				}
			}
			if (UIComponents::buttonBig(ICON_FA_STOP, EditorColor::selection, "Stop")) {
				Runtime::stopGame();
			}
			UIComponents::toggleButton(ICON_FA_FORWARD_STEP, tmp, "Step");
		}
		else {
			if (UIComponents::buttonBig(ICON_FA_PLAY, "Play")) {
				Runtime::startGame();
			}
		}
	}
	UIFlex::endFlex();

	// Spacing
	UIComponents::space(0.0f, 5.0f);

	// Render indicators for game view setup
	/*UIFlex::beginFlex("setup", FlexType::ROW, UIFlex::FULL_WIDTH, 12.0f, Justification::START, Alignment::CENTER, 1.0f);
	{
		UIComponents::space(1.0f, 0.0f);
		UIComponents::tryIcon(ICON_FA_GAUGE);
		UIComponents::label("FPS: " + 0);
	}
	UIFlex::endFlex();*/
}

void GameWindow::renderGameView()
{
	// Prepare game view data
	GameViewPipeline& pipeline = Runtime::getGameViewPipeline();
	bool availableCamera = pipeline.getCameraAvailable();
	uint32_t output = pipeline.getOutput();

	// Warn if camera isn't available
	if (!pipeline.getCameraAvailable()) {
		noCameraIndicator.draw();
		output = 0;
	}

	noCameraIndicator.draw();

	// Render game view
	ImGui::BeginChild("GameView", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		// Check if window is currently being resized
		bool currentlyResizing = currentWindowSize != lastWindowSize;
		if (currentlyResizing) output = 0;

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