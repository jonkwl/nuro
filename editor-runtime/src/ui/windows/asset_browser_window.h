#pragma once

#include <string>

#include "editor_window.h"

struct AssetBrowserFolder {
	std::string name = "Unnamed";
	std::string path = "";

	bool expanded = false;
	std::vector<AssetBrowserFolder> children;
};

class AssetBrowserWindow : public EditorWindow
{
public:
	void render() override;

private:
	// Renders navigation element at given position and returns its size
	ImVec2 renderNavigation(ImDrawList& drawList, ImVec2 position);

	// Renders folder structure element at given position and returns its size
	ImVec2 renderFolderStructure(ImDrawList& drawList, ImVec2 position);

	// Renders folder content element at given position with given size
	void renderFolderContent(ImDrawList& drawList, ImVec2 position, ImVec2 size);

	// Renders an item of the folder structure element
	void renderFolderItem(ImDrawList& drawList, AssetBrowserFolder folder, uint32_t indentation);
};