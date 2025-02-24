#include "asset_browser_window.h"

#include <algorithm>
#include <entt/entt.hpp>

#include "../src/ui/windows/insight_panel_window.h"

AssetBrowserWindow::AssetBrowserWindow() : assetScale(1.0f),
targetAssetScale(1.0f),
folders(),
folderContent(),
selectedFolder(0)
{
	reload();
}

void AssetBrowserWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string title = UIUtils::windowTitle("Asset Browser");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::standard);
	{
		// Get draw list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		// Get window data
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 position = ImGui::GetCursorScreenPos();
		ImVec2 initPosition = ImGui::GetCursorScreenPos();

		// Evaluate inputs
		evaluateInputs();

		//
		// RENDER ELEMENTS
		//
		
		ImVec2 folderStructureSize = renderFolderStructure(drawList, position);
		position.x += folderStructureSize.x;

		ImVec2 navigationSize = renderNavigation(drawList, position);
		position.y += navigationSize.y;

		ImVec2 contentsSize = size - ImVec2(folderStructureSize.x, navigationSize.y);
		renderAssets(drawList, position, contentsSize);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void AssetBrowserWindow::reload()
{
	buildFolders(Runtime::getProject().path(), folders);
	if (folders.size() > 0) selectFolder(&folders[0]);
}

void AssetBrowserWindow::evaluateInputs()
{
	// Get io
	ImGuiIO& io = ImGui::GetIO();

	const float assetScaleSpeed = 0.1f;
	const float assetScaleSmoothing = 6.5f;
	const float assetScaleMin = 1.0f;
	const float assetScaleMax = 3.5f;

	// Inputs only to be evaluated if asset browser is focused or hovered
	if (UIUtils::windowFocused() || UIUtils::windowHovered()) {

		// Inputs only to be evaluated if ctrl is pressed
		if (io.KeyCtrl) {

			// Handle mouse wheel input (asset scale change)
			float relativeScaleStep = assetScale * assetScaleSpeed;
			if (io.MouseWheel > 0) targetAssetScale += relativeScaleStep;
			else if (io.MouseWheel < 0) targetAssetScale -= relativeScaleStep;

		}
	}

	// Apply elastic clamping effect using spring force
	const float elasticity = 12.0f;
	const float damping = 8.0f;

	float overshoot = std::clamp(targetAssetScale, assetScaleMin, assetScaleMax) - targetAssetScale;

	if (overshoot != 0.0f) {
		float normalizedElasticity = elasticity / assetScale;
		float normalizedDamping = damping / assetScale;
		float springForce = overshoot * normalizedElasticity; // (Hooke’s Law: F = -kx)
		float dampingForce = (targetAssetScale - assetScale) * normalizedDamping;
		targetAssetScale += (springForce - dampingForce) * Time::deltaf();
	}

	// Update displayed asset scale using interpolation
	assetScale = glm::mix(assetScale, targetAssetScale, assetScaleSmoothing * Time::deltaf());
}

void AssetBrowserWindow::createAssetUIData(Asset& asset)
{
	//
	// EVALUATE
	//

	asset.uiData.font = EditorUI::getFonts().p;
	asset.uiData.padding = ImVec2(10.0f, 7.0f) * assetScale;
	asset.uiData.iconSize = ImVec2(50.0f, 50.0f) * assetScale;

	asset.uiData.textSize = asset.uiData.font->CalcTextSizeA(asset.uiData.font->FontSize, FLT_MAX, 0.0f, asset.name.c_str());
	asset.uiData.size = asset.uiData.iconSize + asset.uiData.padding * 2.0f + ImVec2(0.0f, asset.uiData.textSize.y + asset.uiData.padding.y);
	
	// Modular width
	// asset.uiData.size.x = std::max(asset.uiData.size.x, asset.uiData.textSize.x + asset.uiData.padding.x * 2.0f);

	// Fixed width
	float xFixedPadding = 13.0f;
	asset.uiData.size.x = asset.uiData.iconSize.x + asset.uiData.padding.x * 2.0f + xFixedPadding;
}

void AssetBrowserWindow::selectFolder(Folder* folder)
{
	selectedFolder = folder;
	if (selectedFolder) {
		selectedFolder->expanded = true;
	}
}

void AssetBrowserWindow::buildFolders(const fs::path& directory, std::vector<Folder>& target, uint32_t i)
{
	if (i == 0) {
		// Clear existing folder structure
		folders.clear();
	}

	// Loop through current directory
	for (const auto& entry : fs::directory_iterator(directory))
	{
		if (!fs::is_directory(entry)) continue;

		Folder folder;

		folder.name = entry.path().filename().string();

		folder.absolutePath = entry.path();
		folder.absolutePathHash = hash(folder.absolutePath);
		folder.relativePath = Runtime::getProject().relative(folder.absolutePath);

		// Recursively build children folders for subdirectory
		buildFolders(entry.path(), folder.children, ++i);

		// Add current folder to list
		target.push_back(folder);
	}
}

void AssetBrowserWindow::buildFolderContent(const fs::path& directory)
{
	// Code is very unoptimized, boilerplate

	FolderContent& content = folderContent[hash(directory)];
	content.assets.clear();

	std::vector<Asset> folders;
	std::vector<Asset> files;

	try {
		// Iterate through directory
		for (const auto& entry : fs::directory_iterator(directory)) {
			// Create new asset
			Asset asset;

			asset.name = entry.path().filename().string();
			asset.absolutePath = entry.path();
			asset.relativePath = Runtime::getProject().relative(asset.absolutePath);

			// Check whether it's a folder or a file
			if (fs::is_directory(entry)) {
				asset.type = Asset::Type::FOLDER;
				folders.push_back(asset);  // Store folders
			}
			else {
				asset.type = Asset::Type::FILE;
				files.push_back(asset);  // Store files
			}
		}

		// Insert all folders first, then all files
		content.assets.insert(content.assets.end(), folders.begin(), folders.end());
		content.assets.insert(content.assets.end(), files.begin(), files.end());

		for (Asset& asset : content.assets) {
			createAssetUIData(asset);
		}
	}
	catch (const fs::filesystem_error& e) {
		Console::out::warning("Project Manager", "Couldn't read directory '" + directory.string() + "'", e.what());
	}
}

uint32_t AssetBrowserWindow::hash(const fs::path& path)
{
	std::string pathString = path.string();
	return entt::hashed_string::value(pathString.c_str());
}

ImVec2 AssetBrowserWindow::renderNavigation(ImDrawList& drawList, ImVec2 position)
{
	//
	// EVALUATE
	//

	const float height = 40.0f;
	const float xPadding = 20.0f;

	ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
	ImVec2 p0 = position;
	ImVec2 p1 = p0 + size;

	ImU32 color = IM_COL32(11, 11, 11, 255);

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, color);

	//
	// DRAW PATH BREADCRUMB
	//

	std::string pathRoot = "No folder selected";
	std::string pathEnd;
	if (selectedFolder) {
		pathRoot = std::string(ICON_FA_FOLDER_OPEN) + "   " + selectedFolder->relativePath.parent_path().string();
		pathEnd = "/" + selectedFolder->name;
	}

	// Evaluate position
	ImFont* baseFont = EditorUI::getFonts().h4;
	ImVec2 textSize = baseFont->CalcTextSizeA(baseFont->FontSize, FLT_MAX, 0.0f, pathRoot.c_str());
	ImVec2 textPosition = ImVec2(position.x + xPadding, position.y + (height - textSize.y) * 0.5f);
	
	// Path root text
	drawList.AddText(baseFont, baseFont->FontSize, textPosition, EditorColor::text_transparent, pathRoot.c_str());

	// Path end text
	textPosition.x += textSize.x;
	drawList.AddText(EditorUI::getFonts().h4_bold, EditorUI::getFonts().h4_bold->FontSize, textPosition, EditorColor::text, pathEnd.c_str());

	return size;
}

ImVec2 AssetBrowserWindow::renderFolderStructure(ImDrawList& drawList, ImVec2 position)
{
	//
	// EVALUATE
	//

	const float width = 240.0f;
	const float xPadding = 20.0f;

	ImVec2 size = ImVec2(width, ImGui::GetContentRegionAvail().y);
	ImVec2 p0 = position;
	ImVec2 p1 = p0 + size;

	ImU32 color = IM_COL32(0, 0, 0, 0);

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, color);

	//
	// DRAW FOLDERS
	//

	IMComponents::beginClippedChild(size, position);
	{
		ImGui::Dummy(ImVec2(0.0f, 8.5f));

		for (Folder& folder : folders) {
			renderFolder(drawList, &folder, 0.0f);
		}

		ImGui::Dummy(ImVec2(0.0f, 8.5f));
	}
	IMComponents::endClippedChild();

	return size;
}

void AssetBrowserWindow::renderAssets(ImDrawList& drawList, ImVec2 position, ImVec2 size)
{
	//
	// EVALUATE
	//

	if (!selectedFolder) return;

	ImVec2 p0 = position;
	ImVec2 p1 = p0 + size;

	ImU32 color = EditorColor::background;

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, color);

	//
	// DRAW ASSETS
	//

	ImVec2 padding = ImVec2(16.0f, 10.0f);
	ImVec2 gap = ImVec2(1.0f, 3.5f);

	IMComponents::beginClippedChild(size, position);
	{
		//
		// DRAW ASSETS
		//

		// Fetch assets to be drawn
		uint32_t pathHash = hash(selectedFolder->absolutePath);

		// Build folder content if it isnt available
		if(folderContent.find(pathHash) == folderContent.end()) buildFolderContent(selectedFolder->absolutePath);

		// 
		std::vector<Asset>& currentAssets = folderContent[pathHash].assets;

		// Initialize cursor with padding (local cursor relative to window)
		ImVec2 cursor = padding;

		ImVec2 lastAssetSize = ImVec2(0.0f, 0.0f);

		for (Asset& asset : currentAssets) {
			// Check for new line
			if (cursor.x + asset.uiData.size.x > size.x - padding.x) {
				// Start new line using current assets height
				cursor.x = padding.x;
				cursor.y += asset.uiData.size.y + gap.y;

				lastAssetSize = asset.uiData.size;
			}

			// Advance internal cursor
			ImGui::SetCursorPos(cursor);

			// Draw asset
			renderAsset(drawList, asset, ImGui::GetCursorScreenPos());

			// Add vertical gap for next asset
			cursor.x += asset.uiData.size.x + gap.x;
		}

		// Add bottom padding
		ImGui::SetCursorPos(cursor + ImVec2(0.0f, padding.y + lastAssetSize.y));
	}
	IMComponents::endClippedChild();
}

void AssetBrowserWindow::renderFolder(ImDrawList& drawList, Folder* folder, uint32_t indentation)
{
	if (!folder) return;

	//
	// ADDITIONALLY GET FOREGROUND DRAW LIST
	//
	ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

	//
	// PROPERTIES
	//
	const float indentationOffset = 26.0f;
	const ImVec2 textPadding = ImVec2(20.0f, 4.5f);

	//
	// EVALUATE
	//

	const bool selected = folder == selectedFolder;
	const bool hasChildren = folder->children.size() > 0;
	const float itemHeight = ImGui::GetFontSize();
	const float textOffset = indentation * indentationOffset;

	const ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
	const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
	const ImVec2 mousePosition = ImGui::GetMousePos();

	const ImVec2 rectMin = ImVec2(cursorPosition.x, cursorPosition.y);
	const ImVec2 rectMax = ImVec2(
		cursorPosition.x + contentRegion.x,
		cursorPosition.y + itemHeight + textPadding.y * 2);
	const ImVec2 finalSize = rectMax - rectMin;

	const bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
	const bool clicked = hovered && ImGui::IsMouseClicked(0);
	const bool doubleClicked = hovered && ImGui::IsMouseDoubleClicked(0);
	const bool wheelClicked = hovered && ImGui::IsMouseClicked(2);
	const bool draggingThis = hovered && ImGui::IsMouseDragging(0);

	//
	// EVALUATE COLOR
	//

	// Base: Standard background color
	ImU32 color = EditorColor::background;
	// Priority #3:  Color when item is hovered
	if (hovered) color = UIUtils::lighten(color, 0.75f);
	// Priority #2: Color when item is selected
	if (selected) color = UIUtils::windowFocused() ? EditorColor::selection : EditorColor::selectionInactive;

	//
	// DRAW ITEM BACKGROUND
	//

	drawList.AddRectFilled(rectMin, rectMax, color, 0.0f);

	//
	// CHECK FOR SELECTION
	//

	if (clicked) selectFolder(folder);

	//
	// DRAW FOLDER ICON
	//

	ImVec2 iconSize = ImVec2(15.0f, 15.0f);
	ImVec2 iconPos = ImVec2(rectMin.x + textPadding.x + textOffset, rectMin.y + (finalSize.y - iconSize.y) * 0.5f);
	drawList.AddImage(IconPool::get("folder"), iconPos, iconPos + iconSize, ImVec2(0, 1), ImVec2(1, 0));

	//
	// EVALUATE ITEM TEXT POSITION
	//

	ImVec2 textPos = ImVec2(iconPos.x + iconSize.x + 10.0f, rectMin.y + textPadding.y);

	//
	// DRAW ITEM CARET CIRCLE
	// 

	if (hasChildren) {
		// Circle geometry
		float circleRadius = 9.0f;
		ImVec2 circlePosition = ImVec2(
			textPos.x + circleRadius - 2.0f,
			textPos.y + circleRadius * 0.5f + 1.0f);

		// Fetch circle interactions
		float circleDistance =
			(mousePosition.x - circlePosition.x) *
			(mousePosition.x - circlePosition.x) +
			(mousePosition.y - circlePosition.y) *
			(mousePosition.y - circlePosition.y);

		bool circleHovered = circleDistance <= (circleRadius * circleRadius);
		bool circleClicked = ImGui::IsMouseClicked(0) && circleHovered;

		// Check for circle click opr mouse wheel click (-> expand)
		if ((circleClicked) || wheelClicked) folder->expanded = !folder->expanded;

		// Evaluate color
		ImU32 circleColor = circleHovered ? (selected ? UIUtils::darken(color, 0.25f) : EditorColor::elementActive) : color;

		// Draw circle
		drawList.AddCircleFilled(circlePosition, circleRadius, circleColor);
	}

	//
	// EVALUATE ICON
	//

	const char* icon = hasChildren ? (folder->expanded ? ICON_FA_CARET_DOWN " " : ICON_FA_CARET_RIGHT " ") : "";

	//
	// DRAW TEXT
	//

	std::string textValue = std::string(icon) + folder->name;
	drawList.AddText(textPos, EditorColor::text, textValue.c_str());

	//
	// ADVANCE CURSOR
	//

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, finalSize.y));
	ImGui::PopStyleVar(2);

	//
	// RENDER CHILDREN
	//

	if (hasChildren && folder->expanded) {
		for (auto& child : folder->children) {
			renderFolder(drawList, &child, indentation + 1);
		}
	}
}

void AssetBrowserWindow::renderAsset(ImDrawList& drawList, Asset& asset, ImVec2 position)
{
	//
	// EVALUATE
	//

	ImU32 textColor = EditorColor::text;

	ImVec2 p0 = position;
	ImVec2 p1 = position + asset.uiData.size;

	bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
	bool selected = false;

	//
	// DRAW BACKGROUND
	//

	ImU32 color = IM_COL32(0, 0, 0, 0);
	if (hovered) color = IM_COL32(255, 255, 255, 12);
	if (selected) color = IM_COL32(85, 110, 255, 80);

	drawList.AddRectFilled(p0, p1, color, 10.0f);

	//
	// DRAW ICON
	//

	uint32_t icon = 0;
	if (asset.thumbnail) icon = asset.thumbnail;
	else {
		switch (asset.type) {
		case Asset::Type::FILE:
			icon = IconPool::get("file");
			break;
		case Asset::Type::FOLDER:
			icon = IconPool::get("folder");
			break;
		case Asset::Type::MATERIAL:
			icon = IconPool::get("material");
			break;
		default:
			icon = IconPool::get("invalid");
			break;
		}
	}

	ImU32 iconColor = asset.loading ? IM_COL32(200, 200, 200, 180) : IM_COL32_WHITE;
	ImVec2 iconPos = ImVec2(position.x + (asset.uiData.size.x - asset.uiData.iconSize.x) * 0.5f, position.y + asset.uiData.padding.y);

	drawList.AddImageRounded(icon, iconPos, iconPos + asset.uiData.iconSize, ImVec2(0, 1), ImVec2(1, 0), iconColor, 10.0f);

	//
	// DRAW LOADING BUFFER IF NEEDED
	//

	if (asset.loading) {
		float bufferSize = asset.uiData.size.x * 0.45f;
		ImVec2 bufferPos = ImVec2(position.x + (asset.uiData.size.x - bufferSize) * 0.5f, position.y + asset.uiData.padding.y + (asset.uiData.iconSize.y - bufferSize) * 0.5f);
		IMComponents::loadingBuffer(drawList, bufferPos, bufferSize * 0.5f, 2, IM_COL32_WHITE);
	}

	//
	// DRAW TEXT
	//

	ImVec2 textPos = ImVec2(position.x + (asset.uiData.size.x - asset.uiData.textSize.x) * 0.5f, iconPos.y + asset.uiData.iconSize.y + asset.uiData.padding.y);
	drawList.AddText(asset.uiData.font, asset.uiData.font->FontSize, textPos, textColor, asset.name.c_str());
}