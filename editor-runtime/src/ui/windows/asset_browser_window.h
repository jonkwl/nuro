#pragma once

#include <string>

#include "editor_window.h"

class AssetBrowserWindow : public EditorWindow
{
public:
	AssetBrowserWindow();

	void render() override;

public:
	enum class AssetType {
		SCENE,
		MODEL,
		MATERIAL,
		SHADER,
		SCRIPT,
		MARKDOWN
	};

	struct Folder {
		std::string name = "Unnamed";
		std::string path = "";

		bool expanded = false;
		std::vector<Folder> children;
	};

	struct Asset {
		std::string name = "Asset";
		std::string path = "";

		AssetType type;

		uint32_t thumbnail = 0;
	};

	struct Icons {
		uint32_t folder = 0;
	};

private:
	// Renders navigation element at given position and returns its size
	ImVec2 renderNavigation(ImDrawList& drawList, ImVec2 position);

	// Renders folder structure element at given position and returns its size
	ImVec2 renderFolderStructure(ImDrawList& drawList, ImVec2 position);

	// Renders folder content element at given position with given size
	void renderFolderContent(ImDrawList& drawList, ImVec2 position, ImVec2 size);

	// Renders a folder item for the folder structure element
	void renderFolderItem(ImDrawList& drawList, Folder folder, uint32_t indentation);

	// Renders an asset item for the folder content element
	ImVec2 renderAssetItem(ImDrawList& drawList, Asset asset, ImVec2 position);

private:
	Icons icons;
};