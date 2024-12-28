#pragma once

#include <string>

#include "window_includes.h"

class HierarchyWindow : public EditorWindow
{
public:
	HierarchyWindow();

	void render();

private:
	void renderSearch(ImDrawList& drawList);
	void renderEntityItems(ImDrawList& drawList);
	void renderItem(ImDrawList& drawList, std::string name, uint32_t indentation, bool children, bool opened, bool selected);

	char searchBuffer[128];
};
