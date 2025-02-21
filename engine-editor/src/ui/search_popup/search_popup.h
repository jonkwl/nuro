#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace SearchPopup
{
	// Renders the current search popup if any is opened
	void render();

	// Closes the search popup if any is opened
	void close();

	// Opens a popup to search for ecs components
	void searchComponents(ImVec2 position);
};