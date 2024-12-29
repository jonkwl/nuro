#include "hierarchy_window.h"

#include "../core/transform/transform.h"

HierarchyWindow::HierarchyWindow() : searchBuffer(""),
currentHierarchy(),
selectedItemId(0),
draggedItem(nullptr)
{
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
	renderDraggedItem(drawList);

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

	// Move camera if needed
	moveCamera();

	// Pop font
	ImGui::PopFont();
}

void HierarchyWindow::renderItem(ImDrawList& drawList, HierarchyItem& item, uint32_t indentation)
{
	// Setup
	const float indentationOffset = 30.0f;
	const ImVec2 textPadding = ImVec2(10.0f, 6.5f);

	const bool selected = item.id == selectedItemId;
	const bool hasChildren = item.children.size() > 0;
	const float textHeight = ImGui::GetFontSize();
	const float textOffset = indentation * indentationOffset;

	// Get the current window position and cursor position
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImVec2 contentRegion = ImGui::GetContentRegionAvail();

	// Draw the filled rectangle
	ImVec2 rectMin = ImVec2(cursorPos.x, cursorPos.y);
	ImVec2 rectMax = ImVec2(cursorPos.x + contentRegion.x, cursorPos.y + textHeight + textPadding.y * 2);
	bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
	bool dropHover = hovered && draggedItem;

	ImU32 color = EditorColor::background;
	if (hovered) UIUtils::lighten(EditorColor::background, 0.38f);
	if (selected) color = EditorColor::selection;
	if (dropHover) color = UIUtils::darken(EditorColor::selection, 0.5f);

	drawList.AddRectFilled(rectMin, rectMax, color, 5.0f);

	// Check for selection
	bool clicked = ImGui::IsMouseClicked(0) && ImGui::GetMousePos().x >= rectMin.x && ImGui::GetMousePos().x <= rectMax.x && ImGui::GetMousePos().y >= rectMin.y && ImGui::GetMousePos().y <= rectMax.y;
	if (clicked) {
		selectedItemId = item.id;
		Runtime::getSceneViewPipeline().setSelectedEntity(&item.entity);
	}

	// Check for moving to entity
	bool doubleClicked = ImGui::IsMouseDoubleClicked(0) && ImGui::GetMousePos().x >= rectMin.x && ImGui::GetMousePos().x <= rectMax.x && ImGui::GetMousePos().y >= rectMin.y && ImGui::GetMousePos().y <= rectMax.y;
	if (doubleClicked) {
		cameraTarget = &item.entity.transform;
		cameraMoving = true;
	}
	if (selected && ImGui::IsKeyPressed(ImGuiKey_F)) {
		cameraTarget = &item.entity.transform;
		cameraMoving = true;
	}

	// Calculate text position
	ImVec2 textPos = ImVec2(rectMin.x + textPadding.x + textOffset, rectMin.y + textPadding.y);

	if (hasChildren) {
		// Draw caret circle
		float circleRadius = 11.0f;
		ImVec2 circlePosition = ImVec2(textPos.x + circleRadius, textPos.y + circleRadius * 0.5f + 1.0f);
		ImVec2 mousePos = ImGui::GetMousePos();
		float distance = (mousePos.x - circlePosition.x) * (mousePos.x - circlePosition.x) + (mousePos.y - circlePosition.y) * (mousePos.y - circlePosition.y);
		bool isHovered = distance <= (circleRadius * circleRadius);
		bool isClicked = ImGui::IsMouseClicked(0) && isHovered;
		if (isClicked) item.expanded = !item.expanded;
		ImU32 circleColor = isHovered && !dropHover ? UIUtils::lighten(color, 1.0f) : color;
		drawList.AddCircleFilled(circlePosition, circleRadius, circleColor);
	}

	// Check for expansion
	bool caretClicked = false;
	if (caretClicked && hasChildren) item.expanded = !item.expanded;

	// Evaluate icon
	const char* icon = hasChildren ? (item.expanded ? " " ICON_FA_CARET_DOWN : " " ICON_FA_CARET_RIGHT) : "";

	// Draw text
	std::string textValue = std::string(icon + ("  " + item.name));
	drawList.AddText(textPos, EditorColor::text, textValue.c_str());

	// Layout dummy
	ImGui::Dummy(ImVec2(contentRegion.x, rectMax.y - rectMin.y - EditorSizing::framePadding * 2 + 2.0f));

	// Render children if item has any and is expanded
	if (hasChildren && item.expanded) {
		for (auto& child : item.children) {
			renderItem(drawList, child, indentation + 1);
		}
	}

	// Check for new drag
	if (hovered && ImGui::IsMouseDragging(0)) {
		if (!draggedItem) {
			draggedItem = &item;
			lastDraggedItemPosition = glm::vec4(rectMin.x, rectMin.y, rectMax.x, rectMax.y);
		}
	}
}

void HierarchyWindow::renderDraggedItem(ImDrawList& drawList)
{
	// Dont proceed if theres no item dragged
	if (!draggedItem) {
		return;
	}

	// Stop drag if mouse isnt hold anymore
	if (!ImGui::IsMouseDown(0)) {
		draggedItem = nullptr;
		return;
	}

	// Setup
	const float width = 100.0f;
	const float height = ImGui::GetFontSize();

	// Get the current window position and cursor position
	ImVec2 mousePos = ImGui::GetMousePos();

	// Draw the filled rectangle
	glm::vec4 position = glm::vec4(mousePos.x, mousePos.y, mousePos.x + width, mousePos.y + height);
	glm::vec4 smoothPosition = glm::mix(lastDraggedItemPosition, position, 15.0f * Time::deltaf());
	drawList.AddRectFilled(ImVec2(smoothPosition.x, smoothPosition.y), ImVec2(smoothPosition.z, smoothPosition.w), EditorColor::selection, 5.0f);

	// Cache last dragged item position
	lastDraggedItemPosition = smoothPosition;
}

void HierarchyWindow::moveCamera()
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
		glm::vec3 positionDelta = newPosition - flyCamera.transform.position;

		glm::quat newRotation = glm::slerp(flyCamera.transform.rotation, targetRotation, t);
		glm::quat rotationDelta = newRotation * glm::inverse(flyCamera.transform.rotation);

		// Set new position and rotation
		/*flyCamera.transform.position = positionDelta;
		flyCamera.transform.rotation = rotationDelta * flyCamera.transform.rotation;*/
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
		currentHierarchy.push_back(HierarchyItem(i, EntityContainer(entity), "Item " + std::to_string(i), {}));
		i++;
	}
}