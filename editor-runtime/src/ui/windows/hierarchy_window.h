#pragma once

#include "../src/ui/editor_window.h"

class HierarchyWindow : public EditorWindow
{
public:
	HierarchyWindow();

	void render();

private:
	void renderSearch();
	void renderItems();

	char searchBuffer[128];
};
