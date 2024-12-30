#pragma once

#include <glm.hpp>

#include "window_includes.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow();
	void render();

private:
	void renderToolbar();
	void renderSceneView();
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
	UIContentRect speedChangeIndicator; // Speed change indicator rect for speed change

	glm::vec2 moveAxis; // Current input axis for evaluating scene view movement
	float moveAxisSmoothingFactor; // Smoothing for key axis
	glm::vec2 mouseCurrent; // Current mouse position
	glm::vec2 mouseLast; // Last mouse position
	glm::vec2 mouseDelta; // Last mouse position

	int32_t gizmoOperation; // Last used gizmo operation
	float gizmoScaleMin; // Limit for downscaling an object with a gizmo

	glm::vec3 cameraEulerAngles; // Cache for cameras euler angles
};