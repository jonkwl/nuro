#pragma once

#include "editor_window.h"

#include "../src/ui/components/toggle_bar.h"

class GameWindow : public EditorWindow
{
public:
	GameWindow();
	void render() override;

private:
	void renderToolbar(ImDrawList& drawList, ImVec2 position);
	void renderGameView();
	void renderForegroundIndicators();

private:
	ImVec2 currentContentAvail; // Current windows size
	ImVec2 lastContentAvail; // Cache last windows size

	ImVec2 lastWindowPosition; // Cache for last window position
	ImVec2 lastWindowSize; // Cache for last window size

	bool windowFocused; // Set if game window is currently focused
	bool windowHovered; // Set if game window is currently hovered

	ImVec4 gameViewBounds; // Bounds of game view (min x, min y, max x, max y)

	DynamicContent noCameraIndicator; // Indicator for when theres no camera to render game view

	ToggleBar playToggles; // Play settings
};