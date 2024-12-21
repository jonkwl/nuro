#pragma once

#include "../src/ui/editor_window.h"
#include "window_includes.h"

class HierarchyWindow : public EditorWindow
{
public:
	HierarchyWindow();

	void render();

private:
	void renderSearch(ImDrawList& drawList);
	void renderItems(ImDrawList& drawList);
	void renderItem(ImDrawList& drawList);

	char searchBuffer[128];
};
