#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "editor_window.h"
#include "../project/project_assets.h"
#include "../project/project_observer.h"

using NodeRef = std::shared_ptr<ProjectObserver::IONode>;
using FileRef = std::shared_ptr<ProjectObserver::File>;
using FolderRef = std::shared_ptr<ProjectObserver::Folder>;

class AssetBrowserWindow : public EditorWindow
{
public:
	AssetBrowserWindow();

	void render() override;

private:
	struct NodeUIData {
		std::string text;
		ImFont* font;
		ImVec2 padding;
		ImVec2 iconSize;
		ImVec2 textSize;
		ImVec2 size;

		explicit NodeUIData(std::string text, ImFont* font, ImVec2 padding, ImVec2 iconSize, ImVec2 textSize, ImVec2 size) :
			text(text), font(font), padding(padding), iconSize(iconSize), textSize(textSize), size(size) 
		{
		};

		static NodeUIData createFor(const std::string& name, float scale = 1.0f);
	};

	// Reference to the global project observer
	ProjectObserver& observer;

	// Reference to the global project assets
	ProjectAssets& assets;

	// Current displayed scale of ui assets
	float assetScale;

	// Target asset scale for smoothly interpolated asset scaling
	float targetAssetScale;

	// Currently selected folders io node id
	uint32_t selectedFolder;

	// Currently selected io nodes id
	uint32_t selectedNode;

	// Checks for asset broser related inputs
	void evaluateInputs();

	// Opens an io node using the system default application
	void openNodeInApplication(const NodeRef& node);

	// Opens an io nodes path using the system default explorer
	void openNodeInExplorer(const NodeRef& node);

	// Selects a folder by its io node id
	void selectFolder(uint32_t folderId);

	// Unselects the currently selected folder if any
	void unselectFolder();

	//
	// RENDERING FUNCTIONS
	//

	// Renders navigation at given position and returns its size
	ImVec2 renderNavigation(ImDrawList& drawList, ImVec2 position);

	// Renders side folder structure at given position and returns its size
	ImVec2 renderSideFolders(ImDrawList& drawList, ImVec2 position);

	// Renders a side folder item for the folder structure
	void renderSideFolder(ImDrawList& drawList, FolderRef folder, uint32_t indentation);

	// Renders all nodes for current selected folder at given position with given size
	void renderNodes(ImDrawList& drawList, ImVec2 position, ImVec2 size);

	// Tries to render a node at the given screen position using its ui data created before
	bool renderNode(ImDrawList& drawList, NodeRef node, const NodeUIData& uiData, ImVec2 position);
};