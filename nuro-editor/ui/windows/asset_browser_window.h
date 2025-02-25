#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <filesystem>
namespace fs = std::filesystem;

#include "editor_window.h"

class AssetBrowserWindow : public EditorWindow
{
public:
	AssetBrowserWindow();

	void render() override;

	// Refetches project paths and reloads asset browser
	void reload();

public:
	struct Asset {
		enum class Type {
			FILE,
			FOLDER,
			SCENE,
			MODEL,
			MATERIAL,
			SHADER,
			SCRIPT,
			MARKDOWN,
		};

		struct UIData {
			ImFont* font = nullptr;
			ImVec2 padding;
			ImVec2 iconSize;
			ImVec2 textSize;
			ImVec2 size;
		};

		std::string name;
		Type type = Type::FILE;

		fs::path absolutePath;
		fs::path relativePath;

		UIData uiData;

		uint32_t thumbnail = 0;
		bool loading = false;
	};

	struct Folder {
		std::string name;

		fs::path absolutePath;
		uint32_t absolutePathHash = 0;
		fs::path relativePath;

		std::vector<Folder> children;

		bool expanded = false;
	};

	struct FolderContent {
		std::vector<Asset> assets;
	};

private:

	//
	// MISC
	// 
	
	// Checks for asset broser related inputs
	void evaluateInputs();

	// Evaluates and sets an assets ui data
	void createAssetUIData(Asset& asset);

	// Selects the given folder (provide nullptr for unselection of current folder)
	void selectFolder(Folder* folder);

	//
	// RENDERING FUNCTIONS
	//

	// Renders navigation at given position and returns its size
	ImVec2 renderNavigation(ImDrawList& drawList, ImVec2 position);

	// Renders folder structure at given position and returns its size
	ImVec2 renderFolderStructure(ImDrawList& drawList, ImVec2 position);

	// Renders asset items for current folder at given position with given size
	void renderAssets(ImDrawList& drawList, ImVec2 position, ImVec2 size);

	// Renders a folder item for the folder structure
	void renderFolder(ImDrawList& drawList, Folder* folder, uint32_t indentation);

	// Renders a ui asset item at the given screen position using its ui data created before
	void renderAsset(ImDrawList& drawList, Asset& asset, ImVec2 position);

	//
	// IO BACKEND FUNCTIONS
	// 
	// Implementing multithreading would be very smart here, 
	// however it's just not a top priority right now.
	//

	// Builds the folder structure for the given directory recursively
	void buildFolders(const fs::path& directory, std::vector<Folder>& target, uint32_t i = 0);

	// Builds the folder content for the given directory path
	void buildFolderContent(const fs::path& directory);

	// Returns the path hashed as an integer
	uint32_t hash(const fs::path& path);

private:
	// Current displayed scale of ui assets
	float assetScale;

	// Target asset scale for smoothly interpolated asset scaling
	float targetAssetScale;

	// Current folder structure for quickly iterating over project folders
	std::vector<Folder> folders;

	// Cached folder content sorted by hash of folders absolute path
	std::unordered_map<uint32_t, FolderContent> folderContent;

	// Pointer to the currently selected folder (nullptr if none selected)
	Folder* selectedFolder;
};