#include "asset_browser_window.h"

void AssetBrowserWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(UIUtils::windowTitle("Asset Browser"), nullptr, EditorFlag::standard);
	{
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 position = ImGui::GetCursorScreenPos();
		
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
	// EVALUATE GEOMETRY
	//

	const float height = 40.0f;
	const float xPadding = 20.0f;

	ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, height);
	ImVec2 p0 = position;
	ImVec2 p1 = p0 + size;

	ImU32 color = IM_COL32(50, 50, 90, 50);

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, color);

	//
	// DRAW PATH BREADCRUMB
	//

	// Example strings
	std::string pathRoot = "Project Folder / Asset Folder / Sub Folder / ";
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
	// EVALUATE GEOMETRY
	//

	const float width = 180.0f;
	const float xPadding = 20.0f;

	ImVec2 size = ImVec2(width, ImGui::GetContentRegionAvail().y);
	ImVec2 p0 = position;
	ImVec2 p1 = p0 + size;

	ImU32 color = IM_COL32(0, 0, 0, 50);

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, color);

	//
	// DRAW FOLDERS
	//

	IMComponents::beginChild(size, position);
	{
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);
		renderFolderItem(drawList, AssetBrowserFolder(), 0.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
	}
	IMComponents::endChild();

	return size;
}

void AssetBrowserWindow::renderFolderContent(ImDrawList& drawList, ImVec2 position, ImVec2 size)
{
	//
	// EVALUATE GEOMETRY
	//

	ImVec2 p0 = position;
	ImVec2 p1 = p0 + size;

	ImU32 color = IM_COL32(255, 90, 90, 10);

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, color);

	//
	// DRAW ASSETS
	//

	IMComponents::beginChild(size, position);
	{
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
		ImGui::Dummy(ImVec2(size.x, 100.0f));
	}
	IMComponents::endChild();
}

void AssetBrowserWindow::renderFolderItem(ImDrawList& drawList, AssetBrowserFolder folder, uint32_t indentation)
{
	//
	// ADDITIONALLY GET FOREGROUND DRAW LIST
	//
	ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

	//
	// PROPERTIES
	//
	const float indentationOffset = 30.0f;
	const ImVec2 textPadding = ImVec2(10.0f, 4.5f);

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
	ImU32 color = IM_COL32(0, 0, 0, 0);
	// Priority #3:  Color when item is hovered
	if (hovered) color = UIUtils::lighten(EditorColor::background, 0.38f);
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
	// EVALUATE ITEM TEXT POSITION
	//

	ImVec2 textPos = ImVec2(rectMin.x + textPadding.x + textOffset, rectMin.y + textPadding.y);

	//
	// DRAW ITEM CARET CIRCLE
	// 

	if (hasChildren) {
		// Circle geometry
		float circleRadius = 9.0f;
		ImVec2 circlePosition = ImVec2(
			textPos.x + circleRadius + 1.0f,
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

	const char* icon = hasChildren ? (folder.expanded ? " " ICON_FA_CARET_DOWN : " " ICON_FA_CARET_RIGHT) : "";

	//
	// DRAW TEXT
	//

	std::string textValue = std::string(icon) + "   " + folder.name;
	drawList.AddText(textPos, EditorColor::text, textValue.c_str());

	//
	// ADVANCE CURSOR
	//

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, finalSize.y - 7.0f));
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