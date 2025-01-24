#include "asset_browser_window.h"

#include <algorithm>

#include "../src/ui/windows/insight_panel_window.h"

AssetBrowserWindow::AssetBrowserWindow() : icons(),
assetScale(1.0f)
{
	// Fetch icon ids
	icons.folder = IconPool::get("folder").getBackendId();
}

void AssetBrowserWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(UIUtils::windowTitle("Asset Browser"), nullptr, EditorFlag::standard);
	{
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 position = ImGui::GetCursorScreenPos();
		ImVec2 initPosition = ImGui::GetCursorScreenPos();

		//
		// RENDER ELEMENTS
		//
		
		ImVec2 folderStructureSize = renderFolderStructure(drawList, position);
		position.x += folderStructureSize.x;

		ImVec2 navigationSize = renderNavigation(drawList, position);
		position.y += navigationSize.y;

		ImVec2 contentsSize = size - ImVec2(folderStructureSize.x, navigationSize.y);
		renderFolderContent(drawList, position, contentsSize);
	}
	ImGui::End();
	ImGui::PopStyleVar();
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

	// Example strings
	std::string pathRoot = std::string(ICON_FA_FOLDER_OPEN) + "   " + "Project Folder / Asset Folder / Sub Folder / ";
	std::string pathEnd = "Current Folder";

	// Evaluate position
	ImFont* baseFont = EditorUI::getFonts().p;
	ImVec2 textSize = baseFont->CalcTextSizeA(baseFont->FontSize, FLT_MAX, 0.0f, pathRoot.c_str());
	ImVec2 textPosition = ImVec2(position.x + xPadding, position.y + (height - textSize.y) * 0.5f);
	
	// Path root text
	drawList.AddText(baseFont, baseFont->FontSize, textPosition, EditorColor::text_transparent, pathRoot.c_str());

	// Path end text
	textPosition.x += textSize.x;
	drawList.AddText(EditorUI::getFonts().p_bold, EditorUI::getFonts().p_bold->FontSize, textPosition, EditorColor::text, pathEnd.c_str());

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

	Folder a;
	a.children.push_back(Folder());
	a.children.push_back(Folder());
	a.children.push_back(Folder());
	a.children.push_back(Folder());
	a.children.push_back(Folder());
	a.expanded = true;

	Folder x;
	x.children.push_back(Folder());
	x.children.push_back(Folder());
	x.expanded = true;
	a.children.push_back(x);

	Folder y;
	y.children.push_back(Folder());
	y.children.push_back(Folder());
	y.expanded = true;
	x.children.push_back(y);

	Folder z;
	z.children.push_back(Folder());
	z.expanded = true;
	y.children.push_back(z);

	Folder w;
	w.children.push_back(Folder());
	w.expanded = true;
	z.children.push_back(w);

	Folder b;
	b.children.push_back(Folder());
	b.expanded = true;

	Folder c;
	c.children.push_back(Folder());
	c.children.push_back(Folder());
	c.children.push_back(Folder());
	c.expanded = true;

	IMComponents::beginChild(size, position);
	{
		ImGui::Dummy(ImVec2(0.0f, 8.5f));

		renderFolderItem(drawList, a, 0.0f);
		renderFolderItem(drawList, b, 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, c, 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);
		renderFolderItem(drawList, Folder(), 0.0f);

		ImGui::Dummy(ImVec2(0.0f, 8.5f));
	}
	IMComponents::endChild();

	return size;
}

void AssetBrowserWindow::renderFolderContent(ImDrawList& drawList, ImVec2 position, ImVec2 size)
{
	//
	// EVALUATE
	//

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

	IMComponents::beginChild(size, position);
	{
		position += ImVec2(16.0f, 10.0f);
		position.x += renderAssetItem(drawList, Asset(), position).x + 1.0f;
		position.x += renderAssetItem(drawList, Asset(), position).x + 1.0f;
		position.x += renderAssetItem(drawList, Asset(), position).x + 1.0f;

		Asset a;
		a.name = "Default Material";
		a.thumbnail = InsightPanelWindow::previewOutput;
		position.x += renderAssetItem(drawList, a, position).x + 1.0f;
	}
	IMComponents::endChild();
}

void AssetBrowserWindow::renderFolderItem(ImDrawList& drawList, Folder folder, uint32_t indentation)
{
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

	const bool selected = false;
	const bool hasChildren = folder.children.size() > 0;
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
	const bool clicked = ImGui::IsMouseClicked(0) && hovered;
	const bool doubleClicked = ImGui::IsMouseDoubleClicked(0) && hovered;
	const bool draggingThis = ImGui::IsMouseDragging(0) && hovered;

	//
	// EVALUATE COLOR
	//

	// Base: Standard background color
	ImU32 color = EditorColor::background;
	// Priority #3:  Color when item is hovered
	if (hovered) color = UIUtils::lighten(color, 0.75f);
	// Priority #2: Color when item is selected
	if (selected) color = ImGui::IsWindowFocused() ? EditorColor::selection : EditorColor::selectionInactive;

	//
	// DRAW ITEM BACKGROUND
	//

	drawList.AddRectFilled(rectMin, rectMax, color, 0.0f);

	//
	// CHECK FOR SELECTION
	//

	if (clicked) {

		/*
		ImGuiIO& io = ImGui::GetIO();

		auto select = [this](HierarchyItem& _item) -> void {
			// Item already selected
			if (selectedItems.find(_item.id) != selectedItems.end()) return;

			// Select item
			selectedItems[_item.id] = &_item;
			Runtime::getSceneViewPipeline().setSelectedEntity(&_item.entity);

			// Update insight panel
			InsightPanelWindow::inspect(new EntityInspectable(_item));
			};

		// Just handle current items selection
		if (io.KeyCtrl) {
			// Item not selected yet, add to selected items
			if (selectedItems.find(item.id) == selectedItems.end()) {
				select(item);
			}
			// Item already selected, remove from selected items
			else {
				selectedItems.erase(item.id);
			}
		}
		// Select all between latest selection and this item
		else if (io.KeyShift) {
			// Make sure last selected is set to prevent memory errors
			if (!lastSelected) lastSelected = &item;

			// Find multiselect start and end elements
			auto start = std::find(currentHierarchy.begin(), currentHierarchy.end(), *lastSelected);
			auto end = std::find(currentHierarchy.begin(), currentHierarchy.end(), item);

			// Select all items between start and end
			if (start != currentHierarchy.end() && end != currentHierarchy.end()) {
				if (start <= end) {
					for (auto i = start; i != end; ++i) {
						select(*i);
					}
					select(*end);
				}
				else {
					for (auto i = start; i != end; --i) {
						select(*i);
					}
					select(*end);
				}
			}
		}
		// Only select this item
		else {
			// If theres multiple selected items, only select this item if its not among the multiple selected ones
			if (selectedItems.size() > 1) {
				if (selectedItems.find(item.id) == selectedItems.end()) {
					selectedItems.clear();
					select(item);
				}
			}
			// Not multiple selected items, just select this item
			else {
				selectedItems.clear();
				select(item);
			}
		}

		// Cache last selected item
		lastSelected = &item;

		*/

	}

	//
	// DRAW FOLDER ICON
	//

	ImVec2 iconSize = ImVec2(15.0f, 15.0f);
	ImVec2 iconPos = ImVec2(rectMin.x + textPadding.x + textOffset, rectMin.y + (finalSize.y - iconSize.y) * 0.5f);
	drawList.AddImage(icons.folder, iconPos, iconPos + iconSize, ImVec2(0, 1), ImVec2(1, 0));

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

		// Check for circle click (-> expand)
		if (circleClicked) folder.expanded = !folder.expanded;

		// Evaluate color
		ImU32 circleColor = circleHovered ? (selected ? UIUtils::darken(color, 0.25f) : EditorColor::elementActive) : color;

		// Draw circle
		drawList.AddCircleFilled(circlePosition, circleRadius, circleColor);
	}

	//
	// EVALUATE ICON
	//

	const char* icon = hasChildren ? (folder.expanded ? ICON_FA_CARET_DOWN " " : ICON_FA_CARET_RIGHT " ") : "";

	//
	// DRAW TEXT
	//

	std::string textValue = std::string(icon) + folder.name;
	drawList.AddText(textPos, EditorColor::text, textValue.c_str());

	//
	// ADVANCE CURSOR
	//

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, finalSize.y - 6.0f));
	ImGui::PopStyleVar();

	//
	// RENDER CHILDREN
	//

	if (hasChildren && folder.expanded) {
		for (auto& child : folder.children) {
			renderFolderItem(drawList, child, indentation + 1);
		}
	}
}

ImVec2 AssetBrowserWindow::renderAssetItem(ImDrawList& drawList, Asset asset, ImVec2 position)
{
	//
	// EVALUATE
	//

	ImVec2 padding = ImVec2(10.0f, 7.0f) * assetScale;
	ImVec2 iconSize = ImVec2(50.0f, 50.0f) * assetScale;
	
	ImFont* font = EditorUI::getFonts().p;
	ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, asset.name.c_str());
	ImU32 textColor = EditorColor::text;

	ImVec2 size = iconSize + padding * 2.0f + ImVec2(0.0f, textSize.y + padding.y);
	size.x = std::max(size.x, textSize.x + padding.x * 2.0f);

	ImVec2 p0 = position;
	ImVec2 p1 = position + size;

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
		// ...
		default:
			icon = icons.folder;
			break;
		}
	}

	ImVec2 iconPos = ImVec2(position.x + (size.x - iconSize.x) * 0.5f, position.y + padding.y);
	drawList.AddImageRounded(icon, iconPos, iconPos + iconSize, ImVec2(0, 1), ImVec2(1, 0), IM_COL32_WHITE, 10.0f);

	//
	// DRAW TEXT
	//

	ImVec2 textPos = ImVec2(position.x + (size.x - textSize.x) * 0.5f, iconPos.y + iconSize.y + padding.y);
	drawList.AddText(font, font->FontSize, textPos, textColor, asset.name.c_str());

	return size;
}
