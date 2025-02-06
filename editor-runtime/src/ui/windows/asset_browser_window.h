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
		FILE,
		FOLDER,
		SCENE,
		MODEL,
		MATERIAL,
		SHADER,
		SCRIPT,
		MARKDOWN,
	};

	struct Folder {
		std::string name;
		std::string path;

		std::vector<Folder> children;

		bool expanded = false;
	};

	// Holds data needed for drawing a ui asset element
	struct AssetUIData {

		// Configuration
		ImFont* font = nullptr;
		ImVec2 padding;
		ImVec2 iconSize;

		// Geometry
		ImVec2 textSize;
		ImVec2 size;

	};

	struct Asset {
		std::string name;
		std::string path;

		AssetType type = AssetType::FILE;
		uint32_t thumbnail = 0;
		bool loading = false;

		AssetUIData uiData;
	};

private:
	// Checks for asset broser related inputs
	void evaluateInputs();

	// Renders navigation at given position and returns its size
	ImVec2 renderNavigation(ImDrawList& drawList, ImVec2 position);

	// Renders folder structure at given position and returns its size
	ImVec2 renderFolderStructure(ImDrawList& drawList, ImVec2 position);

	// Renders asset items for current folder at given position with given size
	void renderAssets(ImDrawList& drawList, ImVec2 position, ImVec2 size);

	// Renders a folder item for the folder structure
	void renderFolderItem(ImDrawList& drawList, Folder& folder, uint32_t indentation);

	// Evaluates and sets an assets ui data
	void createAssetUIData(Asset& asset);

	// Renders a ui asset item at the given screen position using its ui data created before
	void renderAssetItem(ImDrawList& drawList, Asset& asset, ImVec2 position);

private:
	// Current displayed scale of ui assets
	float assetScale;

	// Target asset scale for smoothly interpolated asset scaling
	float targetAssetScale;

	// Currently shown assets from selected folder
	std::vector<Asset> currentAssets;
};