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
#include "../core/rendering/core/transformation.h"
#include "../core/utils/transform.h"
#include "../core/ecs/components.h"

SceneWindow::SceneWindow() : currentWindowSize(glm::vec2(0.0f)),
lastWindowSize(glm::vec2(0.0f)),
windowFocused(false),
windowHovered(false),
sceneViewRightclicked(false),
sceneViewMiddleclicked(false),
sceneViewBounds(0.0f),
movementSpeed(12.0f),
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
	ImGui::Begin("  Scene  ", nullptr, EditorUI::getWindowFlags().standard);
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
	SceneViewPipeline& pipeline = Runtime::sceneViewPipeline;

	// Render toggle buttons for render options
	UILayout::beginFlex("toggles", FlexType::ROW, UILayout::FULL_WIDTH, 40.0f, Justification::CENTER, Alignment::CENTER, 1.0f);
	{
		UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, pipeline.wireframe, "Wireframe");
		UIComponents::toggleButton(ICON_FA_ECLIPSE, pipeline.renderShadows, "Render Shadows");
		UIComponents::toggleButton(ICON_FA_SPARKLES, pipeline.useProfileEffects, "Enable Post Processing");
		UIComponents::toggleButton(ICON_FA_DRAW_SQUARE, pipeline.showGizmos, "Show Gizmos");
		UIComponents::toggleButton(ICON_FA_CLOUDS_SUN, pipeline.showSkybox, "Render Skybox");
	}
	UILayout::endFlex();

	// Render indicators for scene view setup
	UILayout::beginFlex("setup", FlexType::ROW, UILayout::FULL_WIDTH, 12.0f, Justification::START, Alignment::CENTER, 1.0f);
	{
		UIComponents::space(1.0f, 0.0f);
		UIComponents::tryIcon(ICON_FA_GAUGE);
		UIComponents::label("Speed: " + std::to_string(static_cast<int>(movementSpeed)));
	}
	UILayout::endFlex();
}

void SceneWindow::renderSceneView()
{
	SceneViewPipeline& pipeline = Runtime::sceneViewPipeline;
	unsigned int output = pipeline.getOutput();

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

		// Check if scene view is interacted with
		sceneViewRightclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);
		sceneViewMiddleclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Middle);

		// Update cursor bounds and inputs if theres an interaction with scene view
		if (sceneViewRightclicked || sceneViewMiddleclicked) {
			// Make sure cursor is within scene view bounds
			bool positionedCursor = false;
			cursorCurrent = UIUtils::keepCursorInBounds(sceneViewBounds, positionedCursor);
			if (positionedCursor) cursorLast = cursorCurrent;

			// Calculate cursor axis
			cursorDelta = glm::vec2(cursorCurrent.x - cursorLast.x, -(cursorCurrent.y - cursorLast.y));
		}

		// Render scene views transformation gizmos
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
	return;

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

	// Get gizmo matrices
	TransformComponent transform; // Get transform component of selected entity here
	glm::mat4 model = Transformation::model(transform);

	// Check for snapping
	bool snapping = Input::keyDown(Key::LEFT_CONTROL);
	float snapValue = gizmoOperation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f; // Snap to 45 degrees if rotating or 0.5 units otherwise
	const float snapValues[3] = { snapValue, snapValue, snapValue };

	// Draw transformation gizmo
	ImGuizmo::Manipulate(glm::value_ptr(Runtime::sceneViewPipeline.getView()), glm::value_ptr(Runtime::sceneViewPipeline.getProjection()), (ImGuizmo::OPERATION)gizmoOperation, ImGuizmo::MODE::LOCAL, glm::value_ptr(model), nullptr, snapping ? snapValues : nullptr);

	// Update entities transform if gizmo is being used
	if (ImGuizmo::IsUsing()) {
		// Get components from transform matrix
		glm::vec3 position, scale, skew;
		glm::vec4 perspective;
		glm::quat rotation;
		glm::decompose(model, scale, rotation, position, skew, perspective);

		// Update transforms position
		transform.position = Transformation::toBackendPosition(position);

		// Update transforms rotation
		transform.rotation = Transformation::toBackendRotation(rotation);

		// Dont change scale, if its being decreased and is smaller than minimum
		if (scale.x < transform.scale.x && scale.x < gizmoScaleMin) {
			scale.x = transform.scale.x;
		}
		if (scale.y < transform.scale.y && scale.y < gizmoScaleMin) {
			scale.y = transform.scale.y;
		}
		if (scale.z < transform.scale.z && scale.z < gizmoScaleMin) {
			scale.z = transform.scale.z;
		}

		transform.scale = scale;
	}
}

void SceneWindow::updateMovement()
{
	Camera& camera = Runtime::sceneViewPipeline.getFlyCamera();

	// Get values needed
	float deltaTime = Time::deltaf();
	glm::vec3 camForward = Transform::forward(camera.transform);
	glm::vec3 camRight = Transform::right(camera.transform);
	glm::vec3 camUp = Transform::up(camera.transform);

	// Move in scene view
	glm::vec2 currentKeyAxis = !sceneViewRightclicked ? glm::vec2(0.0f) : Input::moveAxis();
	moveAxis = glm::mix(moveAxis, currentKeyAxis, moveAxisSmoothingFactor * deltaTime);
	glm::vec3 movementDir = camForward * moveAxis.x + camRight * moveAxis.y;
	camera.transform.position += movementDir * movementSpeed * deltaTime;

	// If theres a right click interaction with scene view
	if (sceneViewRightclicked) {
		// Check for scrolling movement speed changes
		glm::vec2 currentScrollAxis = Input::scrollDelta();
		movementSpeed = glm::clamp(movementSpeed + currentScrollAxis.y * scrollIncrementSpeed, 1.0f, 100.0f);

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
		camera.transform.position += panningDir * movementSpeed * deltaTime * 0.18f; // 0.18f is a good factor to match movement speed
	}

}