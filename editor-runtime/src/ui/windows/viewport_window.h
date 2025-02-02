#pragma once

#include "editor_window.h"

#include "../src/ui/components/toggle_bar.h"

class ViewportWindow : public EditorWindow
{
public:
	ViewportWindow();
	void render() override;

private:
	void renderSceneView();
	void renderSceneToolbar(ImVec2 position, ImVec2 size);
	void renderTransformGizmos();
	void renderSpeedChangeIndicator();

	void updateMovement(); // Update movement within scene view

private:
	glm::vec2 currentWindowSize; // Current windows size
	glm::vec2 lastWindowSize; // Cache last windows size

	bool windowFocused; // Set if scene window is currently focused
	bool windowHovered; // Set if scene window is currently hovered

	bool sceneViewRightclicked; // Set if scene viewport is currently rightclicked
	bool sceneViewMiddleclicked; // Set if scene viewport is currently rightclicked

	glm::vec4 sceneViewBounds; // Bounds of scene view (min x, min y, max x, max y)

	float movementSpeed; // Speed of camera movement within scene viewport
	float mouseSensitivity; // Scene viewport mouse sensitivity
	float scrollIncrementSpeed; // Speed for incrementing movement speed through mouse scrolling
	float speedChangeTimer; // Timer resetting when the speed is changed
	DynamicContent speedChangeIndicator; // Speed change indicator for speed change

	ToggleBar mainToggles; // Main viewport settings
	ToggleBar playToggles; // Play settings

	bool gamePaused; // Set if game is paused
	bool gameExecuting; // Set if game is executing (running or paused)

	glm::vec2 moveAxis; // Current input axis for evaluating scene view movement
	float moveAxisSmoothingFactor; // Smoothing for key axis
	glm::vec2 mouseCurrent; // Current mouse position
	glm::vec2 mouseLast; // Last mouse position
	glm::vec2 mouseDelta; // Last mouse position

	int32_t gizmoOperation; // Last used gizmo operation
	float gizmoScaleMin; // Limit for downscaling an object with a gizmo
};