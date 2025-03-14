#include "viewport_window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <ImGuizmo.h>
#include <tuple>

#include <time/time.h>
#include <input/input.h>
#include <input/cursor.h>
#include <utils/console.h>
#include <ecs/components.h>
#include <ecs/ecs_collection.h>
#include <transform/transform.h>
#include <rendering/material/lit/lit_material.h>
#include <rendering/transformation/transformation.h>
#include <rendering/postprocessing/post_processing.h>

ViewportWindow::ViewportWindow() : currentWindowSize(glm::vec2(0.0f)),
lastWindowSize(glm::vec2(0.0f)),
windowFocused(false),
windowHovered(false),
sceneViewRightclicked(false),
sceneViewMiddleclicked(false),
sceneViewBounds(0.0f),
movementSpeed(16.0f),
mouseSensitivity(0.08f),
scrollIncrementSpeed(2.0f),
speedChangeTimer(100.0f),
speedChangeIndicator(),
mainToggles(),
playToggles(),
gamePaused(false),
gameExecuting(false),
moveAxis(glm::vec2(0.0f)),
moveAxisSmoothingFactor(5.0f),
mouseCurrent(glm::vec2(0.0f)),
mouseLast(glm::vec2(0.0f)),
mouseDelta(glm::vec2(0.0f)),
gizmoOperation(ImGuizmo::OPERATION::TRANSLATE),
gizmoScaleMin(0.1f)
{
	// Initialize last mouse position
	mouseLast = Cursor::getPosition();

	// Setup speed change indicator
	speedChangeIndicator.color = IM_COL32(0, 0, 0, 0);
	speedChangeIndicator.padding = ImVec2(45.0f, 30.0f);
	speedChangeIndicator.rounding = 25.0f;
	speedChangeIndicator.foreground = true;
	speedChangeIndicator.outline = true;
	speedChangeIndicator.outlineStrength = 1.0f;
	speedChangeIndicator.outlineColor = IM_COL32(255, 255, 255, 125);
	speedChangeIndicator.horizontalAlignment = Horizontal::CENTER;
	speedChangeIndicator.verticalAlignment = Vertical::CENTER;
	DynamicText speedChangeText(EditorUI::getFonts().h3);
	speedChangeText.color = IM_COL32(255, 255, 255, 255);
	speedChangeText.alignment = TextAlign::CENTER;
	speedChangeIndicator.addText(speedChangeText);

	// Setup main toggles
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();
	mainToggles.addItem(ICON_FA_CUBE, pipeline.wireframe);
	mainToggles.addItem(ICON_FA_ECLIPSE, pipeline.renderingShadows);
	mainToggles.addItem(ICON_FA_SUN, pipeline.showSkybox);
	mainToggles.addItem(ICON_FA_SPARKLES, pipeline.useProfileEffects);
	mainToggles.addItem(ICON_FA_DRAW_SQUARE, pipeline.showGizmos);

	// Setup play toggles
	static bool tmp = false;
	ToggleBarStyle& playTogglesStyle = playToggles.getStyle();
	playTogglesStyle.padding = ImVec2(16.0f, 10.0f);
	playTogglesStyle.font = EditorUI::getFonts().s;
	playToggles.addItem(ICON_FA_PAUSE, gamePaused, true);
	playToggles.addItem(ICON_FA_PLAY, gameExecuting, true);
	playToggles.addItem(ICON_FA_SQUARE, tmp, true);
	playToggles.getItemStyle(0).selectedColor = IM_COL32(255, 130, 65, 200);
	playToggles.getItemStyle(1).selectedColor = IM_COL32(85, 125, 255, 200);

}

void ViewportWindow::render()
{
	mouseCurrent = Cursor::getPosition();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string title = UIUtils::windowTitle("Viewport");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::standard);
	{

		windowFocused = UIUtils::windowFocused();
		windowHovered = UIUtils::windowHovered();

		// Get scene window size
		currentWindowSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

		// Render scene view
		renderSceneView();

	}

	// Try render speed change indicator
	renderSpeedChangeIndicator();

	ImGui::End();
	ImGui::PopStyleVar();

	mouseLast = mouseCurrent;

	// Interaction with scene view
	if (sceneViewRightclicked || sceneViewMiddleclicked) {
		// Hide cursor
		EditorUI::setCursorMode(CursorMode::HIDDEN);

		// Set scene window to focused
		ImGui::SetWindowFocus();
	}

	// Update movement according to inputs calculated prior
	updateMovement();
}

void ViewportWindow::renderSceneView()
{
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();
	uint32_t output = pipeline.getOutput();

	// Get position of scene view
	ImVec2 sceneViewPosition = ImGui::GetCursorScreenPos();

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
	ImVec2 sceneViewSize = ImVec2(width, height);

	// Check if scene view is interacted with
	sceneViewRightclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);
	sceneViewMiddleclicked = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Middle);

	// Update cursor bounds and inputs if theres an interaction with scene view
	if (sceneViewRightclicked || sceneViewMiddleclicked) {

		// Make sure cursor is within scene view bounds
		bool cursorMoved = false;
		mouseCurrent = UIUtils::keepCursorInBounds(sceneViewBounds, cursorMoved);
		if (cursorMoved) mouseLast = mouseCurrent;

		// Calculate cursor axis
		mouseDelta = glm::vec2(mouseCurrent.x - mouseLast.x, -(mouseCurrent.y - mouseLast.y));

	}

	// Render transform gizmos
	renderTransformGizmos();

	// Check if scene window has been resized
	if (currentlyResizing && !Input::mouseDown(MouseButton::LEFT)) {
		pipeline.resizeViewport(width, height);
		lastWindowSize = currentWindowSize;
	}

	// Render scene view settings
	renderSceneToolbar(sceneViewPosition, sceneViewSize);
}

void ViewportWindow::renderSceneToolbar(ImVec2 position, ImVec2 size)
{
	// Get foreground draw list and pipeline
	ImDrawList& drawList = *ImGui::GetWindowDrawList();
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();

	// Evaluate game state values
	GameState gameState = Runtime::getGameState();
	gamePaused = gameState == GameState::GAME_PAUSED;
	gameExecuting = gameState == GameState::GAME_RUNNING || gamePaused;

	float padding = 22.0f;

	//
	// MAIN TOGGLE BAR
	//

	ImVec2 p0 = position + ImVec2(padding, padding);
	mainToggles.render(drawList, p0);

	// 
	// PLAY TOGGLE BAR
	//

	p0 = position + ImVec2(size.x * 0.5f - playToggles.getSize().x * 0.5f, padding);
	playToggles.render(drawList, p0);

	// Evaluate pause clicked
	if (playToggles.itemClicked(0)) {
		if (!gamePaused) Runtime::pauseGame();
		else Runtime::continueGame();
	}
	
	// Evaluate play clicked
	if (playToggles.itemClicked(1)) {
		if (!gameExecuting) Runtime::startGame();
		else Runtime::stopGame();
	}

	//
	// SETTINGS BUTTON
	//

	p0 = position + ImVec2(size.x - 50.0f, padding);
	IMComponents::toggleButton(drawList, ICON_FA_GEAR, pipeline.useProfileEffects, "Settings", p0);

	//
	// LABELS
	//
	p0 = position + ImVec2(padding, 55.0f);
	std::string speedText = std::string(ICON_FA_GAUGE) + " Speed: " + std::to_string(static_cast<int32_t>(movementSpeed));
	drawList.AddText(EditorUI::getFonts().s, EditorSizing::s_FontSize, p0, IM_COL32(255, 255, 255, 190), speedText.c_str());
}

void ViewportWindow::renderTransformGizmos()
{
	// Fetch selected entities
	const std::vector<EntityContainer*>& selectedEntities = Runtime::getSceneViewPipeline().getSelectedEntities();
	if (selectedEntities.empty()) return;
	EntityContainer* selected = selectedEntities[0];

	// Dont render transform gizmos if scene view is interacted with
	if (sceneViewRightclicked || sceneViewMiddleclicked) return;

	// Setup gizmo viewport
	ImVec2 itemPosition = ImGui::GetItemRectMin();
	ImVec2 itemSize = ImGui::GetItemRectSize();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(itemPosition.x, itemPosition.y, itemSize.x, itemSize.y);

	// Change gizmo operation
	if (windowFocused) {
		if (Input::command(Key::T)) gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		else if (Input::command(Key::R)) gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		else if (Input::command(Key::S)) gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	// Copy transforms model matrix
	TransformComponent& transform = selected->get<TransformComponent>();
	glm::mat4 model = transform.model;

	// Snapping
	bool snapping = Input::keyDown(Key::LEFT_CONTROL);
	float snapValue = gizmoOperation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f; // Snap 0.5 units or 45 degrees for rotating
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

	// Update entities transform if gizmo is used
	if (ImGuizmo::IsUsing()) {
		// Get components from transformation
		glm::vec3 positionDelta, scaleDelta, skewDelta;
		glm::vec4 perspectiveDelta;
		glm::quat rotationDelta;
		glm::decompose(model, scaleDelta, rotationDelta, positionDelta, skewDelta, perspectiveDelta);

		// Update position
		Transform::setPosition(transform, Transformation::swap(positionDelta), Space::WORLD);

		// Update rotation
		Transform::setRotation(transform, Transformation::swap(glm::normalize(rotationDelta)), Space::WORLD);

		// Update scale
		/*glm::vec3 newScale = transform.scale * scaleDelta;
		newScale.x = (newScale.x < transform.scale.x && newScale.x < gizmoScaleMin) ? transform.scale.x : newScale.x;
		newScale.y = (newScale.y < transform.scale.y && newScale.y < gizmoScaleMin) ? transform.scale.y : newScale.y;
		newScale.z = (newScale.z < transform.scale.z && newScale.z < gizmoScaleMin) ? transform.scale.z : newScale.z;
		newScale = glm::max(newScale, glm::min(transform.scale, gizmoScaleMin));
		Transform::scale(transform, newScale);*/
	}
}

void ViewportWindow::renderSpeedChangeIndicator()
{
	// Properties
	const uint32_t visibleAlpha = 200;
	
	// Determine visibility
	bool visible = false;
	speedChangeTimer += Time::deltaf();
	if (speedChangeTimer < 0.5f) visible = true;

	// Set alpha color according to visibility
	speedChangeIndicator.color = IM_COL32(0, 0, 10, visible ? visibleAlpha : 0);

	// Set text
	std::string icon = std::string(ICON_FA_GAUGE);
	speedChangeIndicator.modifyText(0).text = icon + "   " + std::to_string(movementSpeed);
	
	// Update and draw speed change indicator
	speedChangeIndicator.draw();
}

void ViewportWindow::updateMovement()
{
	SceneViewPipeline& pipeline = Runtime::getSceneViewPipeline();
	TransformComponent& cameraTransform = std::get<0>(pipeline.getFlyCamera());

	// Get values needed
	float delta = Time::deltaf();
	glm::vec3 camForward = Transform::forward(cameraTransform);
	glm::vec3 camRight = Transform::right(cameraTransform);
	glm::vec3 camUp = Transform::up(cameraTransform);

	// Move in scene view
	glm::vec2 currentKeyAxis = !sceneViewRightclicked ? glm::vec2(0.0f) : Input::moveAxis();
	moveAxis = glm::mix(moveAxis, currentKeyAxis, moveAxisSmoothingFactor * delta);
	glm::vec3 movementDir = camForward * moveAxis.x + camRight * moveAxis.y;
	cameraTransform.position += movementDir * movementSpeed * delta;

	// If theres a right click interaction with scene view
	if (sceneViewRightclicked) {
		// Check for scrolling movement speed changes
		float currentScroll = ImGui::GetIO().MouseWheel;
		if (currentScroll) {
			movementSpeed = glm::clamp(movementSpeed + currentScroll * scrollIncrementSpeed, 1.0f, 100.0f);
			speedChangeTimer = 0.0f;
		}

		// Look around
		glm::vec3 rotationDir = glm::vec3(-mouseDelta.y, mouseDelta.x, 0.0f);
		glm::vec3 newRotation = Transform::getEulerAngles(cameraTransform) + (rotationDir * mouseSensitivity);
		Transform::setEulerAngles(cameraTransform, newRotation);
	}

	// If theres a middle click interaction with scene view
	if (sceneViewMiddleclicked) {
		// Panning in scene view
		glm::vec3 panningDir = (camRight * -mouseDelta.x) + (camUp * -mouseDelta.y);
		cameraTransform.position += panningDir * movementSpeed * delta * 0.1f; // 0.1f is a good factor to match movement speed
	}

	// Evaluate if fly camera moved this frame
	const float movementThreshold = 0.001f;
	bool moving = std::abs(movementDir.x) > movementThreshold || std::abs(movementDir.y) > movementThreshold || std::abs(movementDir.z) > movementThreshold;

}