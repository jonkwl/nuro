#include "hierarchy_window.h"

#include <algorithm>

#include "../core/transform/transform.h"

enum DropType {
	NO_DROP,
	DROP_ITEM,
	MOVE_ITEM_UP,
	MOVE_ITEM_DOWN
};

HierarchyWindow::HierarchyWindow() : searchBuffer(""),
contextMenuUsed(false),
currentHierarchy(),
selectedItems(),
lastSelected(nullptr),
lastHovered(nullptr),
dragRect(),
draggingHierarchy(false),
cameraMoving(false),
cameraMovementTime(0.0f),
cameraTarget(nullptr)
{
	// Setup drag rect
	dragRect.padding = ImVec2(20.0f, 10.0f);
	dragRect.foreground = true;
	dragRect.rounding = 5.0f;
	dragRect.color = EditorColor::selection;
	dragRect.lastColor = EditorColor::selection;
	DynamicText dragRectText(EditorUI::getFonts().uiBold);
	dragRectText.color = IM_COL32(255, 255, 255, 255);
	dragRectText.alignment = TextAlign::CENTER;
	dragRect.addText(dragRectText);
}

void HierarchyWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::Begin(UIUtils::windowTitle("Hierarchy"), nullptr, EditorFlag::fixed);
	{
		renderContextMenu();

		// Get draw list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		UIComponents::headline("Hierarchy", ICON_FA_SITEMAP, HeadlineJustification::LEFT);

		renderSearch(drawList);
		renderHierarchy(drawList);
		renderDraggedItem();

		performAutoScroll();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void HierarchyWindow::renderSearch(ImDrawList& drawList)
{
	const float padding = 8.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

	if (ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		Log::printWarning("Hierarchy Window", "Search not implemented yet");
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	UIComponents::space(0.0f, 5.0f);
}

void HierarchyWindow::renderHierarchy(ImDrawList& drawList)
{
	// tmp
	static bool builtHierarchy = false;
	if (!builtHierarchy) {
		buildSceneHierarchy();
		builtHierarchy = true;
	}

	// Push font
	ImGui::PushFont(EditorUI::getFonts().uiBold);

	// Render hierarchy
	for (auto& item : currentHierarchy) {
		renderItem(drawList, item, 0);
	}

	// Update camera movement
	updateCameraMovement();

	// Pop font
	ImGui::PopFont();
}

void HierarchyWindow::renderItem(ImDrawList& drawList, HierarchyItem& item, uint32_t indentation)
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

	const bool selected = selectedItems.count(item.id) > 0;
	const bool hasChildren = item.children.size() > 0;
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

	const bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax) && !contextMenuUsed;
	const bool clicked = ImGui::IsMouseClicked(0) && hovered;
	const bool doubleClicked = ImGui::IsMouseDoubleClicked(0) && hovered;
	const bool draggingThis = ImGui::IsMouseDragging(0) && hovered;

	if (hovered) lastHovered = &item;

	//
	// CHECK FOR DROP TYPE ON THIS ITEM IF SOME ITEM IS CURRENTLY BEING DRAGGED
	//

	DropType dropType = NO_DROP;
	if (hovered && draggingHierarchy) {
		// Mouse in the top quarter of item element (-> move item up)
		if (mousePosition.y < rectMin.y + finalSize.y * 0.25f) {
			dropType = MOVE_ITEM_UP;
			// Mouse in the bottom quarter of item element (-> move item down)
		}
		else if (mousePosition.y > rectMax.y - finalSize.y * 0.25f) {
			dropType = MOVE_ITEM_DOWN;
			// Mouse in the middle of item element (-> drop item)
		}
		else {
			dropType = DROP_ITEM;
		}
	}

	//
	// CHECK FOR MOVING ITEM DISPLAY
	//

	ImU32 moveLineColor = EditorColor::selection;
	const float moveLineThickness = 1.0f;
	const float moveLineOffet = 2.0f;
	switch (dropType) {
	case MOVE_ITEM_UP:
		foregroundDrawList->AddLine(
			ImVec2(rectMin.x, rectMin.y - moveLineOffet),
			ImVec2(rectMax.x, rectMin.y - moveLineOffet),
			moveLineColor,
			moveLineThickness);
		break;
	case MOVE_ITEM_DOWN:
		foregroundDrawList->AddLine(ImVec2(
			rectMin.x, rectMax.y + moveLineOffet),
			ImVec2(rectMax.x, rectMax.y + moveLineOffet),
			moveLineColor,
			moveLineThickness);
		break;
	}

	//
	// EVALUATE COLOR
	//

	// Base:
	ImU32 color = EditorColor::background;
	// Priority #3:
	if (hovered) color = UIUtils::lighten(EditorColor::background, 0.38f);
	// Priority #2:
	if (selected) color = EditorColor::selection;
	// Priority #1:
	if (dropType == DROP_ITEM) color = UIUtils::darken(EditorColor::selection, 0.5f);

	//
	// DRAW ITEM BACKGROUND
	//

	drawList.AddRectFilled(rectMin, rectMax, color, 5.0f);

	//
	// CHECK FOR SELECTION
	//

	if (clicked) {

		ImGuiIO& io = ImGui::GetIO();

		auto select = [this](HierarchyItem& _item) -> void {
			// Item already selected
			if (selectedItems.find(_item.id) != selectedItems.end()) return;

			// Select item
			selectedItems[_item.id] = &_item;
			Runtime::getSceneViewPipeline().setSelectedEntity(&_item.entity);   
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

	}

	//
	// CHECK FOR DOUBLE CLICK (-> move to entity)
	//

	if (doubleClicked) {
		setCameraTarget(&item.entity.transform);
	}

	//
	// CHECK FOR F KEYPRESS WHEN SELECTED (-> move to entity)
	// 

	if (selected && ImGui::IsKeyPressed(ImGuiKey_F)) {
		setCameraTarget(&item.entity.transform);
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
		if (circleClicked) item.expanded = !item.expanded;
		
		// Evaluate color
		ImU32 circleColor = circleHovered && dropType == NO_DROP ? (selected ? UIUtils::darken(color, 0.25f) : EditorColor::elementActive) : color;
		
		// Draw circle
		drawList.AddCircleFilled(circlePosition, circleRadius, circleColor);
	}

	//
	// EVALUATE ICON
	//

	const char* icon = hasChildren ? (item.expanded ? " " ICON_FA_CARET_DOWN : " " ICON_FA_CARET_RIGHT) : "";

	//
	// DRAW TEXT
	//

	std::string textValue = std::string(icon) + "   " + item.entity.name;
	drawList.AddText(textPos, EditorColor::text, textValue.c_str());

	//
	// ADVANCE CURSOR
	//

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, finalSize.y - 7.0f));
	ImGui::PopStyleVar();

	//
	// CHECK FOR BEGINNING TO DRAG THIS ITEM
	//

	if (draggingThis && !draggingHierarchy) {
		draggingHierarchy = true;

		// Update drag rect text
		size_t nSelectedItems = selectedItems.size();
		std::string text = nSelectedItems > 1 ? std::to_string(selectedItems.size()) + " selected" : "Selected " + item.entity.name;
		std::string icon(ICON_FA_LEFT_LONG);
		dragRect.modifyText(0).text = icon + "   " + text;

		// Match drag rects geometry to items geometry for a smooth transition
		dragRect.lastPosition = ImVec2(cursorPosition.x, cursorPosition.y);
		dragRect.lastSize = ImVec2(contentRegion.x, itemHeight + textPadding.y * 2);
	}

	//
	// CHECK FOR ENDING DRAG ON THIS ITEM (-> DROPPING HERE)
	//

	if (draggingHierarchy && !ImGui::IsMouseDown(0) && dropType != NO_DROP) {
		switch (dropType) {
		case DROP_ITEM:
			// Drop dragged item here action
			break;
		case MOVE_ITEM_UP:
			// Move dragged item up action
			break;
		case MOVE_ITEM_DOWN:
			// Movw dragged item down action
			break;
		}
	}

	//
	// RENDER CHILDREN
	//

	if (hasChildren && item.expanded) {
		for (auto& child : item.children) {
			renderItem(drawList, child, indentation + 1);
		}
	}
}

void HierarchyWindow::renderDraggedItem()
{
	// Dont proceed if theres no item being dragged
	if (!draggingHierarchy) {
		return;
	}

	// If not dragging anymore, stop
	if (!ImGui::IsMouseDown(0)) {
		draggingHierarchy = false;
	}

	// Draw drag rect
	dragRect.position = ImGui::GetMousePos() + ImVec2(12.0f, 12.0f);
	dragRect.draw();
}

void HierarchyWindow::renderContextMenu()
{
	if (ContextMenu::begin())
	{
		contextMenuUsed = true;

		if (lastHovered) {
			if (ContextMenu::item(ICON_FA_TRASH, "Delete"))
			{
				// Handle delete
			}

			if (ContextMenu::item(ICON_FA_COPY, "Duplicate"))
			{
				// Handle duplicate
			}

			if (ContextMenu::item(ICON_FA_PENCIL, "Rename"))
			{
				// Handle rename
			}

			ContextMenu::seperator();
		}

		if (ContextMenu::item(ICON_FA_LAYER_PLUS, "Empty Entity"))
		{
			// Handle empty entity
		}

		if (ContextMenu::item(ICON_FA_CAMERA_MOVIE, "Camera"))
		{
			// Handle camera
		}

		if (ContextMenu::menu(ICON_FA_CUBE, "3D Primitives"))
		{
			if (ContextMenu::itemLight("Cube")) { /* Handle cube */ }
			if (ContextMenu::itemLight("Sphere")) { /* Handle sphere */ }
			if (ContextMenu::itemLight("Capsule")) { /* Handle capsule */ }
			if (ContextMenu::itemLight("Cylinder")) { /* Handle cylinder */ }
			if (ContextMenu::itemLight("Pyramid")) { /* Handle pyramid */ }
			if (ContextMenu::itemLight("Plane")) { /* Handle plane */ }

			ContextMenu::endMenu();
		}

		if (ContextMenu::menu(ICON_FA_LIGHTBULB, "Light"))
		{
			if (ContextMenu::itemLight("Directional Light")) { /* Handle directional light */ }
			if (ContextMenu::itemLight("Point Light")) { /* Handle point light */ }
			if (ContextMenu::itemLight("Spotlight")) { /* Handle spotlight */ }

			ContextMenu::endMenu();
		}

		if (ContextMenu::menu(ICON_FA_VOLUME_HIGH, "Audio"))
		{
			if (ContextMenu::itemLight("Audio Source")) { /* Handle audio source */ }

			ContextMenu::endMenu();
		}

		ContextMenu::end();
	}
	else {
		contextMenuUsed = false;
		lastHovered = nullptr;
	}

	ContextMenu::pop();
}

void HierarchyWindow::updateCameraMovement()
{
	if (!cameraMoving) return;

	if (!cameraTarget) return;

	// Get fly camera
	Camera& flyCamera = Runtime::getSceneViewPipeline().getFlyCamera();

	// Get target transform
	TransformComponent& targetTransform = *cameraTarget;

	// Get targets
	float distance = 5.0f + cameraTarget->scale.z;
	glm::vec3 targetPosition = targetTransform.position + Transform::backward(targetTransform) * distance;
	glm::quat targetRotation = Transform::lookFromAt(targetPosition, targetTransform);

	float duration = 0.5f;
	if (cameraMovementTime < duration) {
		// Calculate position and rotation delta
		float t = glm::clamp(cameraMovementTime / duration, 0.0f, 1.0f);

		// Get smoothed targets
		glm::vec3 newPosition = glm::mix(flyCamera.transform.position, targetPosition, t);
		glm::quat newRotation = glm::slerp(flyCamera.transform.rotation, targetRotation, t);

		// Set new position and rotation
		flyCamera.transform.position = newPosition;
		flyCamera.transform.rotation = newRotation;

		// Add to elapsed camera movement time
		cameraMovementTime += Time::deltaf();
	}
	else {
		// Stop camera movement
		flyCamera.transform.position = targetPosition;
		flyCamera.transform.rotation = targetRotation;

		// Reset
		cameraMoving = false;
		cameraMovementTime = 0.0f;
	}
}

void HierarchyWindow::buildSceneHierarchy()
{
	currentHierarchy.clear();

	auto transforms = ECS::gRegistry.view<TransformComponent>();
	uint32_t i = 1;
	for (auto [entity, transform] : transforms.each()) {
		currentHierarchy.push_back(HierarchyItem(i, EntityContainer("Item " + std::to_string(i), entity), {}));
		i++;
	}
	currentHierarchy.pop_back();

	int _i = 0;
	for (auto [entity, transform] : transforms.each()) {
		if (_i > 0 && _i < 11) {
			currentHierarchy[_i].children.push_back(HierarchyItem(i + _i + 1, EntityContainer("Child " + std::to_string(_i), entity), {}));
		}
		_i++;
	}
}

void HierarchyWindow::setCameraTarget(TransformComponent* target)
{
	cameraTarget = target;
	cameraMoving = true;
}

void HierarchyWindow::performAutoScroll()
{
	// No item dragged -> no auto scroll
	if (!draggingHierarchy) return;

	// Properties
	const float maxScrollSpeed = 45.0f;
	const float scrollArea = 0.3f;

	// Get data
	float mouseY = ImGui::GetMousePos().y;
	float windowY = ImGui::GetWindowPos().y;
	float windowHeight = ImGui::GetWindowHeight();

	auto rangeFactor = [](float x, const float range[2]) -> float {
		return 1.0f - glm::clamp((x - range[0]) / (range[1] - range[0]), 0.0f, 1.0f);
	};

	// Scroll up
	float upRange[2] = { windowY, windowY + windowHeight * scrollArea };
	if (mouseY < upRange[1]) {
		float scrollSpeed = maxScrollSpeed * rangeFactor(mouseY, upRange);
		ImGui::SetScrollY(ImGui::GetScrollY() - scrollSpeed);
		return;
	}

	// Scroll down
	float downRange[2] = { windowY + windowHeight, windowY + windowHeight * (1.0f - scrollArea) };
	if (mouseY > downRange[1]) {
		float scrollSpeed = maxScrollSpeed * rangeFactor(mouseY, downRange);
		ImGui::SetScrollY(ImGui::GetScrollY() + scrollSpeed);
	}
}