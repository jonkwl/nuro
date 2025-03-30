#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <filesystem>
namespace fs = std::filesystem;

#include "editor_window.h"
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
		ImFont* font;
		ImVec2 padding;
		ImVec2 iconSize;
		ImVec2 textSize;
		ImVec2 size;

		explicit NodeUIData(ImFont* font, ImVec2 padding, ImVec2 iconSize, ImVec2 textSize, ImVec2 size) :
			font(font), padding(padding), iconSize(iconSize), textSize(textSize), size(size) 
		{
		};

		static NodeUIData createFor(const std::string& name, float scale = 1.0f);
	};

	// Reference to the global project observer
	ProjectObserver& observer;

	// Current displayed scale of ui assets
	float assetScale;

	// Target asset scale for smoothly interpolated asset scaling
	float targetAssetScale;

	// IO ID of currently selected folder
	uint32_t selectedFolder;

	// Checks for asset broser related inputs
	void evaluateInputs();

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

	// Renders a node at the given screen position using its ui data created before
	void renderNode(ImDrawList& drawList, NodeRef node, const NodeUIData& uiData, ImVec2 position);
};