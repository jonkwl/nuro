#include "scene_window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/type_ptr.hpp>
#include <gtx/matrix_decompose.hpp>

#include <ImGuizmo.h>

#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/input/input.h"
#include "../core/input/cursor.h"
#include "../core/time/time.h"
#include "../core/utils/log.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/rendering/core/transformation.h"
#include "../core/transform/transform.h"
#include "../core/ecs/components.h"
#include "../core/rendering/material/lit/lit_material.h"

SceneWindow::SceneWindow() : currentWindowSize(glm::vec2(0.0f)),
lastWindowSize(glm::vec2(0.0f)),
windowFocused(false),
windowHovered(false),
sceneViewRightclicked(false),
sceneViewMiddleclicked(false),
sceneViewBounds(0.0f),
movementSpeed(16.0f),
mouseSensitivity(0.08f),
scrollIncrementSpeed(2.0f),
moveAxis(glm::vec2(0.0f)),
moveAxisSmoothingFactor(5.0f),
cursorCurrent(glm::vec2(0.0f)),
cursorLast(glm::vec2(0.0f)),
cursorDelta(glm::vec2(0.0f)),
gizmoOperation(ImGuizmo::OPERATION::TRANSLATE),
gizmoScaleMin(0.1f),
cameraEulerAngles(glm::vec3(0.0f))
{
	cursorLast = Cursor::getPosition();
}

void SceneWindow::render()
{
	cursorCurrent = Cursor::getPosition();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(UIUtils::windowTitle("Scene"), nullptr, EditorFlag::standard);
	{
		windowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

		UIComponents::space(0.0f, 14.0f);
		UIComponents::headline("Scene", ICON_FA_MAP, HeadlineJustification::CENTER);

		// Get scene window size
		currentWindowSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

		// Render scene windows toolbar
		renderToolbar();

		// Render scene view
		renderSceneView();

	}
	ImGui::End();
	ImGui::PopStyleVar();

	cursorLast = cursorCurrent;

	// Hide cursor if theres an interaction with scene view
	if (sceneViewRightclicked || sceneViewMiddleclicked) EditorUI::setCursorMode(CursorMode::HIDDEN);

	// Update movement according to inputs calculated prior
	updateMovement();
}

void SceneWindow::renderToolbar()
{
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();

	// Render toggle buttons for render options
	UIFlex::beginFlex("toggles", FlexType::ROW, UIFlex::FULL_WIDTH, 40.0f, Justification::CENTER, Alignment::CENTER, 1.0f);
	{
		UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, pipeline.wireframe, "Wireframe");
		UIComponents::toggleButton(ICON_FA_ECLIPSE, pipeline.renderShadows, "Render Shadows");
		UIComponents::toggleButton(ICON_FA_SPARKLES, pipeline.useProfileEffects, "Enable Post Processing");
		UIComponents::toggleButton(ICON_FA_DRAW_SQUARE, pipeline.showGizmos, "Show Gizmos");
		UIComponents::toggleButton(ICON_FA_CLOUDS_SUN, pipeline.showSkybox, "Render Skybox");
		UIComponents::toggleButton(ICON_FA_ROTATE, pipeline.alwaysUpdate, "Always Update");
	}
	UIFlex::endFlex();

	// Render indicators for scene view setup
	UIFlex::beginFlex("setup", FlexType::ROW, UIFlex::FULL_WIDTH, 12.0f, Justification::START, Alignment::CENTER, 1.0f);
	{
		UIComponents::space(1.0f, 0.0f);
		UIComponents::tryIcon(ICON_FA_GAUGE);
		UIComponents::label("Speed: " + std::to_string(static_cast<int32_t>(movementSpeed)));
	}
	UIFlex::endFlex();
}

void SceneWindow::renderSceneView()
{
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();
	uint32_t output = pipeline.getOutput();

	ImGui::BeginChild("SceneView", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		// Check if window is currently being resized
		bool currentlyResizing = currentWindowSize != lastWindowSize;
		if (currentlyResizing) output = 0;

		// Render target
		ImGui::Image(output, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

		// Get scene view bounds
		ImVec2 boundsMin = ImGui::GetItemRectMin();
		ImVec2 boundsMax = ImGui::GetItemRectMax();
		sceneViewBounds = glm::vec4(boundsMin.x, boundsMin.y, boundsMax.x, boundsMax.y);

		// Get scene view width and height
		float width = boundsMax.x - boundsMin.x;
		float height = boundsMax.y - boundsMin.y;

		// Check for new rightclick interaction to sync current cameras rotation euler angles
		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && !sceneViewRightclicked) {
			cameraEulerAngles = glm::degrees(glm::eulerAngles(pipeline.getFlyCamera().transform.rotation));
		}

		// Check if scene view is interacted with
		sceneViewRightclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);
		sceneViewMiddleclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Middle);

		// Update cursor bounds and inputs if theres an interaction with scene view
		if (sceneViewRightclicked || sceneViewMiddleclicked) {
			// Set scene pipeline to interacted
			pipeline.setUpdated();

			// Make sure cursor is within scene view bounds
			bool cursorMoved = false;
			cursorCurrent = UIUtils::keepCursorInBounds(sceneViewBounds, cursorMoved);
			if (cursorMoved) cursorLast = cursorCurrent;

			// Calculate cursor axis
			cursorDelta = glm::vec2(cursorCurrent.x - cursorLast.x, -(cursorCurrent.y - cursorLast.y));
		}

		// Render transform gizmos
		renderTransformGizmos();

		// Check if scene window has been resized
		if (currentlyResizing && !Input::mouseDown(MouseButton::LEFT)) {
			pipeline.resizeViewport(width, height);
			lastWindowSize = currentWindowSize;
		}
	}
	ImGui::EndChild();
}

void SceneWindow::renderTransformGizmos()
{
	const std::vector<EntityContainer*>& selectedEntities = Runtime::getSceneViewPipeline().getSelectedEntities();

	if (selectedEntities.size() < 1) return;

	EntityContainer* selected = selectedEntities[0];

	// Dont render transform gizmos if scene view is interacted with
	if (sceneViewRightclicked || sceneViewMiddleclicked) return;

	// Get boundaries
	ImVec2 itemPosition = ImGui::GetItemRectMin();
	ImVec2 itemSize = ImGui::GetItemRectSize();
	float itemX = itemPosition.x;
	float itemY = itemPosition.y;
	float itemWidth = itemSize.x;
	float itemHeight = itemSize.y;

	// Setup imguizmo
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(itemX, itemY, itemWidth, itemHeight);

	// Change gizmo operation if issued
	if (windowFocused) {
		if (Input::command(Key::T)) {
			gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		}
		else if (Input::command(Key::R)) {
			gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		}
		else if (Input::command(Key::S)) {
			gizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
	}

	// Get gizmo model matrix
	TransformComponent& transform = selected->get<TransformComponent>();
	glm::mat4 model = Transformation::model(transform);
	if (transform.parent) model = transform.parent->model * model;

	// Check for snapping
	bool snapping = Input::keyDown(Key::LEFT_CONTROL);
	float snapValue = gizmoOperation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f; // Snap to 45 degrees if rotating or 0.5 units otherwise
	const float snapValues[3] = { snapValue, snapValue, snapValue };

	// Draw transformation gizmo
	glm::mat4 delta = glm::mat4(1.0f);
	ImGuizmo::Manipulate(
		glm::value_ptr(Runtime::getSceneViewPipeline().getView()), 
		glm::value_ptr(Runtime::getSceneViewPipeline().getProjection()), 
		(ImGuizmo::OPERATION)gizmoOperation, 
		ImGuizmo::MODE::LOCAL, 
		glm::value_ptr(model), 
		glm::value_ptr(delta),
		snapping ? snapValues : nullptr);

	// Update entities transform if gizmo is being used
	if (ImGuizmo::IsUsing()) {
		// Get components from transform matrix
		glm::vec3 positionDelta, scaleDelta, skewDelta;
		glm::vec4 perspectiveDelta;
		glm::quat rotationDelta;
		glm::decompose(delta, scaleDelta, rotationDelta, positionDelta, skewDelta, perspectiveDelta);

		// Update transforms position
		transform.position += Transformation::toBackendPosition(positionDelta);

		// Update transforms rotation
		// transform.rotation = Transformation::toBackendRotation(rotationDelta) * transform.rotation;

		// Get transforms new scale
		glm::vec3 newScale = transform.scale * scaleDelta;

		// Dont change scale axis, if its being decreased and is smaller than minimum
		if (newScale.x < transform.scale.x && newScale.x < gizmoScaleMin) {
			newScale.x = transform.scale.x;
		}
		if (newScale.y < transform.scale.y && newScale.y < gizmoScaleMin) {
			newScale.y = transform.scale.y;
		}
		if (newScale.z < transform.scale.z && newScale.z < gizmoScaleMin) {
			newScale.z = transform.scale.z;
		}

		// Apply new scale
		transform.scale = newScale;
	}
}

void SceneWindow::updateMovement()
{
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();
	Camera& camera = pipeline.getFlyCamera();

	// Get values needed
	float delta = Time::deltaf();
	glm::vec3 camForward = Transform::forward(camera.transform);
	glm::vec3 camRight = Transform::right(camera.transform);
	glm::vec3 camUp = Transform::up(camera.transform);

	// Move in scene view
	glm::vec2 currentKeyAxis = !sceneViewRightclicked ? glm::vec2(0.0f) : Input::moveAxis();
	moveAxis = glm::mix(moveAxis, currentKeyAxis, moveAxisSmoothingFactor * delta);
	glm::vec3 movementDir = camForward * moveAxis.x + camRight * moveAxis.y;
	camera.transform.position += movementDir * movementSpeed * delta;

	// If theres a right click interaction with scene view
	if (sceneViewRightclicked) {
		// Check for scrolling movement speed changes
		float currentScroll = ImGui::GetIO().MouseWheel;
		movementSpeed = glm::clamp(movementSpeed + currentScroll * scrollIncrementSpeed, 1.0f, 100.0f);

		// Rotate in scene view
		glm::vec3 rotationDir = glm::vec3(-cursorDelta.y, cursorDelta.x, 0.0f);
		glm::vec3 newRotation = cameraEulerAngles + (rotationDir * mouseSensitivity);
		newRotation = glm::vec3(glm::clamp(newRotation.x, -90.0f, 90.0f), newRotation.y, newRotation.z);
		camera.transform.rotation = glm::quat(glm::radians(newRotation));
		cameraEulerAngles = newRotation;
	}

	// If theres a middle click interaction with scene view
	if (sceneViewMiddleclicked) {
		// Panning in scene view
		glm::vec3 panningDir = (camRight * -cursorDelta.x) + (camUp * -cursorDelta.y);
		camera.transform.position += panningDir * movementSpeed * delta * 0.1f; // 0.1f is a good factor to match movement speed
	}

	// Evaluate if fly camera moved this frame
	const float movementThreshold = 0.001f;
	bool moving = std::abs(movementDir.x) > movementThreshold ||
		std::abs(movementDir.y) > movementThreshold ||
		std::abs(movementDir.z) > movementThreshold;
	if (moving) pipeline.setUpdated();

}