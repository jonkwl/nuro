#include "scene_window.h"

#include <imgui.h>

#include "../src/editor/editor_ui.h"
#include "../src/editor/ui_layout.h"
#include "../src/editor/ui_components.h"
#include "../src/editor/IconsFontAwesome6.h"
#include "../src/runtime/runtime.h"
#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/input/input.h"
#include "../src/input/cursor.h"
#include "../src/time/time.h"
#include "../src/utils/log.h"

SceneWindow::SceneWindow() : lastContentRegionAvail(glm::vec2(0.0f)),
sceneViewRightclicked(false),
movementSpeed(12.0f),
mouseSensitivity(0.08f),
scrollIncrementSpeed(2.0f),
keyAxis(glm::vec2(0.0f)),
keyAxisSmoothingFactor(5.0f),
mouseCurrent(glm::vec2(0.0f)),
mouseLast(glm::vec2(0.0f)),
mouseAxis(glm::vec2(0.0f))
{
	Runtime::getCamera().transform.position.y = 2.0f;
	Runtime::getCamera().transform.rotation.x = 22.0f;

	mouseLast = Cursor::getPosition();
}

void SceneWindow::render()
{
	mouseCurrent = Cursor::getPosition();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("  Scene  ", nullptr, EditorUI::getWindowFlags().standard);
	{

		bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

		UIComponents::space(0.0f, 14.0f);
		UIComponents::headline("Scene", ICON_FA_MAP, HeadlineJustification::CENTER);

		glm::vec2 contentRegionAvail = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

		// Check if scene view has been resized
		if (contentRegionAvail != lastContentRegionAvail && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			Runtime::resizeViewport(contentRegionAvail.x, contentRegionAvail.y);

			lastContentRegionAvail = contentRegionAvail;
		}

		renderToolbar();
		renderSceneView();

	}
	ImGui::End();
	ImGui::PopStyleVar();

	mouseLast = mouseCurrent;

	updateMovement(Runtime::getCamera());
}

void SceneWindow::renderToolbar()
{
	UILayout::beginFlex("toggles", FlexType::ROW, UILayout::FULL_WIDTH, 40.0f, Justification::CENTER, Alignment::CENTER, 1.0f);
	{
		UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, Runtime::wireframe, "Wireframe");
		UIComponents::toggleButton(ICON_FA_CUBE, Runtime::solidMode, "Solid Mode");
		UIComponents::toggleButton(ICON_FA_ECLIPSE, Runtime::shadows, "Shadows");
		UIComponents::toggleButton(ICON_FA_SPARKLES, Runtime::postProcessingEffects, "Post Processing");
	}
	UILayout::endFlex();

	UILayout::beginFlex("setup", FlexType::ROW, UILayout::FULL_WIDTH, 13.0f, Justification::START, Alignment::CENTER, 1.0f);
	{
		UIComponents::space(1.0f, 0.0f);
		UIComponents::label("Speed: " + std::to_string(static_cast<int>(movementSpeed)));
	}
	UILayout::endFlex();
}

void SceneWindow::renderSceneView()
{
	ImGui::BeginChild("SceneView", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImGui::Image(
			Runtime::postProcessingPipeline.getOutput(),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0));

		sceneViewRightclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);

		if (sceneViewRightclicked) {
			EditorUI::hideCursor();

			ImVec2 boundsMin = ImGui::GetItemRectMin();
			ImVec2 boundsMax = ImGui::GetItemRectMax();

			bool positionedCursor = false;
			mouseCurrent = checkCursorBoundaries(glm::vec2(boundsMin.x, boundsMin.y), glm::vec2(boundsMax.x, boundsMax.y), positionedCursor);
			if (positionedCursor) mouseLast = mouseCurrent;
			mouseAxis = glm::vec2(mouseCurrent.x - mouseLast.x, -(mouseCurrent.y - mouseLast.y));
		}

	}
	ImGui::EndChild();
}

void SceneWindow::updateMovement(Camera& camera)
{
	// Check for scrolling movement speed changes
	glm::vec2 currentScrollAxis = Input::getScrollAxis();
	movementSpeed = glm::clamp(movementSpeed + currentScrollAxis.y * scrollIncrementSpeed, 1.0f, 100.0f);

	// Move in scene view
	glm::vec2 currentKeyAxis = !sceneViewRightclicked ? glm::vec2(0.0f) : Input::getKeyAxis();
	keyAxis = glm::mix(keyAxis, currentKeyAxis, keyAxisSmoothingFactor * Time::getDeltaTime());
	glm::vec3 cam_forward = camera.transform.forward();
	glm::vec3 cam_right = camera.transform.right();
	glm::vec3 movement_direction = cam_forward * keyAxis.x + cam_right * keyAxis.y;
	camera.transform.position += movement_direction * movementSpeed * Time::fgetDeltaTime();

	// Rotate in scene view
	if (sceneViewRightclicked) {
		glm::vec3 rotate_direction = glm::vec3(-mouseAxis.y, mouseAxis.x, 0.0f);
		glm::vec3 new_rotation = camera.transform.rotation + (rotate_direction * mouseSensitivity);
		new_rotation = glm::vec3(glm::clamp(new_rotation.x, -90.0f, 90.0f), new_rotation.y, new_rotation.z);
		camera.transform.rotation = new_rotation;
	}
}

glm::vec2 SceneWindow::checkCursorBoundaries(glm::vec2 min, glm::vec2 max, bool& positionedCursor)
{
	// Offset preventing immediate wrapping at boundary
	float offset = 10.0f;

	glm::vec2 currentPosition = Cursor::getPosition();
	glm::vec2 updatedPosition = currentPosition;

	// Horizontal boundaries
	if (currentPosition.x < min.x + offset) {
		updatedPosition = glm::vec2(max.x - offset, currentPosition.y);
		Cursor::setPosition(updatedPosition);
		positionedCursor = true;
	}
	else if (currentPosition.x > max.x - offset) {
		updatedPosition = glm::vec2(min.x + offset, currentPosition.y);
		Cursor::setPosition(updatedPosition);
		positionedCursor = true;
	}

	// Vertical boundaries
	if (currentPosition.y < min.y + offset) {
		updatedPosition = glm::vec2(currentPosition.x, max.y - offset);
		Cursor::setPosition(updatedPosition);
		positionedCursor = true;
	}
	else if (currentPosition.y > max.y - offset) {
		updatedPosition = glm::vec2(currentPosition.x, min.y + offset);
		Cursor::setPosition(updatedPosition);
		positionedCursor = true;
	}

	return updatedPosition;
}