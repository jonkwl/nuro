#include "game_window.h"

#include <imgui.h>
#include <ImGuizmo.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/type_ptr.hpp>
#include <gtx/matrix_decompose.hpp>

#include "../src/runtime/runtime.h"
#include "../src/ui/editor_ui.h"
#include "../src/ui/ui_layout.h"
#include "../src/ui/ui_utils.h"
#include "../src/ui/ui_components.h"
#include "../src/ui/IconsFontAwesome6.h"

#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/input/input.h"
#include "../core/input/cursor.h"
#include "../core/time/time.h"
#include "../core/utils/log.h"
#include "../core/rendering/core/transformation.h"

#include "../src/runtime/tmp_context.h"

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
		UIComponents::headline(ICON_FA_PLAY "Game", ICON_FA_MAP, HeadlineJustification::CENTER);

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
	ImGui::BeginChild("GameView", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		// Check if window is currently being resized
		bool currentlyResizing = currentWindowSize != lastWindowSize;

		// Get content region avail for game view
		ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

		// Calculate desired aspect ratio (16:9)
		float aspectRatio = 16.0f / 9.0f;

		// Determine the maximum size that maintains the 16:9 aspect ratio
		float width = contentRegionAvail.x;
		float height = contentRegionAvail.x / aspectRatio;

		// Calculated height exceeds available height, scale by height instead
		if (height > contentRegionAvail.y) {
			height = contentRegionAvail.y;
			width = contentRegionAvail.y * aspectRatio;
		}

		// Set offset of game view render target to make sure its centered
		ImVec2 offset = ImVec2((contentRegionAvail.x - width) * 0.5f, (contentRegionAvail.y - height) * 0.5f);
		ImGui::SetCursorPos(offset);

		// Render target
		ImGui::Image(currentlyResizing ? 0 : Runtime::postProcessingPipeline.getOutput(), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

		ImVec2 boundsMin = ImGui::GetItemRectMin();
		ImVec2 boundsMax = ImGui::GetItemRectMax();
		gameViewBounds = glm::vec4(boundsMin.x, boundsMin.y, boundsMax.x, boundsMax.y);

		// UIUtils::keepCursorInBounds(sceneViewBounds, positionedCursor);

		// Check if game window has been resized
		if (currentlyResizing && !Input::mouseDown(MouseButton::LEFT)) {
			Runtime::resizeViewport(width, height);
			lastWindowSize = currentWindowSize;
		}
	}
	ImGui::EndChild();
}