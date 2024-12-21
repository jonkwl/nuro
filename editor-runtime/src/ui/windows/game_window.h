#pragma once

#include <glm.hpp>

#include "../src/ui/editor_window.h"
#include "window_includes.h"

class GameWindow : public EditorWindow
{
public:
	GameWindow();
	void render();

private:
	void renderToolbar();
	void renderGameView();

private:
	glm::vec2 currentWindowSize; // Current windows size
	glm::vec2 lastWindowSize; // Cache last windows size

	bool windowFocused; // Set if game window is currently focused
	bool windowHovered; // Set if game window is currently hovered

	glm::vec4 gameViewBounds; // Bounds of game view (min x, min y, max x, max y)
};