#include "hierarchy_window.h"

#include "../core/transform/transform.h"

enum DropType {
	NO_DROP,
	DROP_ITEM,
	MOVE_ITEM_UP,
	MOVE_ITEM_DOWN
};

HierarchyWindow::HierarchyWindow() : searchBuffer(""),
currentHierarchy(),
selectedItemId(0),
dragRect(),
draggedItem(nullptr)
{
	//
	// SETUP DRAG RECT
	//

	dragRect.padding = ImVec2(20.0f, 10.0f);
	dragRect.foreground = true;
	dragRect.rounding = 5.0f;
	dragRect.color = EditorColor::selection;
	dragRect.lastColor = EditorColor::selection;

	UIText dragRectText(EditorUI::getFonts().uiBold);
	dragRectText.color = IM_COL32(255, 255, 255, 255);
	dragRectText.alignment = ALIGN_CENTER;
	dragRect.addText(dragRectText);

}

void HierarchyWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::Begin(UIUtils::windowTitle("Hierarchy"), nullptr, EditorFlag::fixed);

	// Get draw list
	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	UIComponents::headline("Hierarchy", ICON_FA_SITEMAP, HeadlineJustification::LEFT);

	renderSearch(drawList);
	renderHierarchy(drawList);
	renderDraggedItem();

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
	buildSceneHierarchy();

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
	const ImVec2 textPadding = ImVec2(10.0f, 6.5f);

	//
	// EVALUATE
	//

	const bool selected = item.id == selectedItemId;
	const bool hasChildren = item.children.size() > 0;
	const float itemHeight = ImGui::GetFontSize();
	const float textOffset = indentation * indentationOffset;

	const ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
	const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
	const ImVec2 mousePosition = ImGui::GetMousePos();

	const ImVec2 rectMin = ImVec2(cursorPosition.x, cursorPosition.y);
	const ImVec2 rectMax = ImVec2(cursorPosition.x + contentRegion.x, cursorPosition.y + itemHeight + textPadding.y * 2);
	const ImVec2 finalSize = rectMax - rectMin;

	const bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
	const bool clicked = ImGui::IsMouseClicked(0) && hovered;
	const bool doubleClicked = ImGui::IsMouseDoubleClicked(0) && hovered;
	const bool dragging = ImGui::IsMouseDragging(0) && hovered;

	//
	// CHECK FOR DROP TYPE ON THIS ITEM IF SOME ITEM IS CURRENTLY BEING DRAGGED
	//

	DropType dropType = NO_DROP;
	if (hovered && draggedItem) {
		// Mouse in the top quarter of item element (-> move item up)
		if (mousePosition.y < rectMin.y + finalSize.y * 0.25f) {
			dropType = MOVE_ITEM_UP;
		// Mouse in the bottom quarter of item element (-> move item down)
		} else if (mousePosition.y > rectMax.y - finalSize.y * 0.25f) {
			dropType = MOVE_ITEM_DOWN;
		// Mouse in the middle of item element (-> drop item)
		} else {
			dropType = DROP_ITEM;
		}
	}

	//
	// CHECK FOR MOVING ITEM DISPLAY
	//

	const float moveLineThickness = 2.0f;
	switch (dropType) {
	case MOVE_ITEM_UP:
		foregroundDrawList->AddLine(ImVec2(rectMin.x, rectMin.y - 3.0f), ImVec2(rectMax.x, rectMin.y - 3.0f), IM_COL32(255, 255, 255, 255), moveLineThickness);
		break;
	case MOVE_ITEM_DOWN:
		foregroundDrawList->AddLine(ImVec2(rectMin.x, rectMax.y + 3.0f), ImVec2(rectMax.x, rectMax.y + 3.0f), IM_COL32(255, 255, 255, 255), moveLineThickness);
		break;
	}

	//
	// EVALUATE COLOR
	//

	// Base:
	ImU32 color = EditorColor::background;
	// Priority #3:
	if (hovered) UIUtils::lighten(EditorColor::background, 0.38f);
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
		selectedItemId = item.id;
		Runtime::getSceneViewPipeline().setSelectedEntity(&item.entity);
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
		float circleRadius = 11.0f;
		ImVec2 circlePosition = ImVec2(textPos.x + circleRadius, textPos.y + circleRadius * 0.5f + 1.0f);

		// Fetch circle interactions
		float circleDistance = (mousePosition.x - circlePosition.x) * (mousePosition.x - circlePosition.x) + (mousePosition.y - circlePosition.y) * (mousePosition.y - circlePosition.y);
		bool circleHovered = circleDistance <= (circleRadius * circleRadius);
		bool circleClicked = ImGui::IsMouseClicked(0) && circleHovered;
		
		// Check for circle click (-> expand)
		if (circleClicked) item.expanded = !item.expanded;
		
		// Evaluate color
		ImU32 circleColor = circleHovered && dropType == NO_DROP ? UIUtils::lighten(color, 1.0f) : color;
		
		// Draw circle
		drawList.AddCircleFilled(circlePosition, circleRadius, circleColor);
	}

	//
	// EVALUATE ICON
	//

	const char* icon1 = hasChildren ? (item.expanded ? " " ICON_FA_CARET_DOWN : " " ICON_FA_CARET_RIGHT) : "";
	const char* icon2 = dropType == DROP_ITEM ? "   " ICON_FA_OCTAGON_PLUS : "";

	//
	// DRAW TEXT
	//

	std::string textValue = std::string(icon1) + std::string(icon2) + "    " + item.entity.name;
	drawList.AddText(textPos, EditorColor::text, textValue.c_str());

	//
	// ADVANCE CURSOR
	//

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Dummy(ImVec2(contentRegion.x, finalSize.y));
	ImGui::PopStyleVar();

	//
	// CHECK FOR BEGINNING TO DRAG THIS ITEM
	//

	if (dragging && !draggedItem) {
		// Update dragged item
		draggedItem = &item;

		// Update drag rect text
		std::string icon(ICON_FA_LEFT_LONG);
		dragRect.getText(0)->text = icon + "   " + draggedItem->entity.name;

		// Match drag rects geometry to items geometry for a smooth transition
		dragRect.lastPosition = ImVec2(cursorPosition.x, cursorPosition.y);
		dragRect.lastSize = ImVec2(contentRegion.x, itemHeight + textPadding.y * 2);
	}

	//
	// CHECK FOR ENDING DRAG ON THIS ITEM (-> DROPPING HERE)
	//

	if (draggedItem && !ImGui::IsMouseDown(0) && dropType != NO_DROP) {
		draggedItem = nullptr;

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
	if (!draggedItem) {
		return;
	}

	dragRect.position = ImGui::GetMousePos() + ImVec2(12.0f, 12.0f);
	dragRect.update();
	dragRect.draw();
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
}

void HierarchyWindow::setCameraTarget(TransformComponent* target)
{
	cameraTarget = target;
	cameraMoving = true;
}
