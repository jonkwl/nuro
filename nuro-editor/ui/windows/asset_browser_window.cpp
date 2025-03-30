#include "asset_browser_window.h"

#include <algorithm>
#include <entt/entt.hpp>

#include "../ui/windows/insight_panel_window.h"

AssetBrowserWindow::AssetBrowserWindow() : observer(Runtime::projectManager().observer()),
assetScale(1.0f),
targetAssetScale(1.0f),
selectedFolder(0)
{
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
		
		ImVec2 folderStructureSize = renderSideFolders(drawList, position);
		position.x += folderStructureSize.x;

		ImVec2 navigationSize = renderNavigation(drawList, position);
		position.y += navigationSize.y;

		ImVec2 contentsSize = size - ImVec2(folderStructureSize.x, navigationSize.y);
		renderNodes(drawList, position, contentsSize);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

AssetBrowserWindow::NodeUIData AssetBrowserWindow::NodeUIData::createFor(const std::string& name, float scale)
{
	//
	// EVALUATE
	//

	ImFont* font = EditorUI::getFonts().p;
	ImVec2 padding = ImVec2(10.0f, 7.0f) * scale;
	ImVec2 iconSize = ImVec2(50.0f, 50.0f) * scale;

	ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, name.c_str());
	ImVec2 size = iconSize + padding * 2.0f + ImVec2(0.0f, textSize.y + padding.y);

	// Modular width
	// size.x = std::max(size.x, textSize.x + padding.x * 2.0f);

	// Fixed width
	float xFixedPadding = 13.0f;
	size.x = iconSize.x + padding.x * 2.0f + xFixedPadding;

	return NodeUIData(font, padding, iconSize, textSize, size);
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
	std::string pathEnd = "";

	if (selectedFolder) {
		auto folder = observer.fetchFolder(selectedFolder);
		if (folder) {
			pathRoot = std::string(ICON_FA_FOLDER_OPEN) + "   " + folder->get()->path.parent_path().string();
			pathEnd = "\\" + folder->get()->name;
		}
		else selectedFolder = 0;
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

ImVec2 AssetBrowserWindow::renderSideFolders(ImDrawList& drawList, ImVec2 position)
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

		// FOLDERS

		for (auto& folder : observer.rootNode()->subfolders) {
			renderSideFolder(drawList, folder, 0.0f);
		}

		ImGui::Dummy(ImVec2(0.0f, 8.5f));
	}
	IMComponents::endClippedChild();

	return size;
}

void AssetBrowserWindow::renderSideFolder(ImDrawList& drawList, FolderRef folder, uint32_t indentation)
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

	const bool selected = folder->ioId == selectedFolder;
	const bool hasChildren = folder->hasSubfolders();
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

	if (clicked) selectedFolder = folder->ioId;

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
		for (auto& child : folder->subfolders) {
			renderSideFolder(drawList, child, indentation + 1);
		}
	}
}

void AssetBrowserWindow::renderNodes(ImDrawList& drawList, ImVec2 position, ImVec2 size)
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
		// DRAW FILES
		//

		ImVec2 cursor = padding;
		ImVec2 lastAssetSize = ImVec2(0.0f, 0.0f);

		auto folder = observer.fetchFolder(selectedFolder);
		if (!folder) {
			selectedFolder = 0;
			return;
		}

		auto _node = [&](const NodeRef& node) {
			// Create ui data
			NodeUIData uiData = NodeUIData::createFor(node->name, assetScale);

			// Start new line using current assets height if needed
			if (cursor.x + uiData.size.x > size.x - padding.x) {
				cursor.x = padding.x;
				cursor.y += uiData.size.y + gap.y;
				lastAssetSize = uiData.size;
			}

			// Advance internal cursor
			ImGui::SetCursorPos(cursor);

			// Draw asset
			renderNode(drawList, node, uiData, ImGui::GetCursorScreenPos());

			// Add vertical gap for next asset
			cursor.x += uiData.size.x + gap.x;
		};

		for (const auto& subfolder : folder->get()->subfolders) {
			_node(subfolder);
		}

		for (const auto& file : folder->get()->files) {
			_node(file);
		}

		// Bottom padding
		ImGui::SetCursorPos(cursor + ImVec2(0.0f, padding.y + lastAssetSize.y));
	}
	IMComponents::endClippedChild();
}

void AssetBrowserWindow::renderNode(ImDrawList& drawList, NodeRef node, const NodeUIData& uiData, ImVec2 position)
{
	//
	// EVALUATE
	//

	ImU32 textColor = EditorColor::text;

	ImVec2 p0 = position;
	ImVec2 p1 = position + uiData.size;

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
	if (node->isFolder())
		icon = IconPool::get("folder");
	else
		icon = IconPool::get("file");

	bool loading = false;

	ImU32 iconColor = loading ? IM_COL32(200, 200, 200, 180) : IM_COL32_WHITE;
	ImVec2 iconPos = ImVec2(position.x + (uiData.size.x - uiData.iconSize.x) * 0.5f, position.y + uiData.padding.y);

	drawList.AddImageRounded(icon, iconPos, iconPos + uiData.iconSize, ImVec2(0, 1), ImVec2(1, 0), iconColor, 10.0f);

	//
	// DRAW LOADING BUFFER IF NEEDED
	//

	if (loading) {
		float bufferSize = uiData.size.x * 0.45f;
		ImVec2 bufferPos = ImVec2(position.x + (uiData.size.x - bufferSize) * 0.5f, position.y + uiData.padding.y + (uiData.iconSize.y - bufferSize) * 0.5f);
		IMComponents::loadingBuffer(drawList, bufferPos, bufferSize * 0.5f, 2, IM_COL32_WHITE);
	}

	//
	// DRAW TEXT
	//

	ImVec2 textPos = ImVec2(position.x + (uiData.size.x - uiData.textSize.x) * 0.5f, iconPos.y + uiData.iconSize.y + uiData.padding.y);
	drawList.AddText(uiData.font, uiData.font->FontSize, textPos, textColor, node->name.c_str());
}