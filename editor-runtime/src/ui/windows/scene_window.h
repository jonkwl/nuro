#pragma once

#include <glm.hpp>

#include "../src/ui/editor_window.h"
#include "../core/camera/camera.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow();
	void render();

private:
	void renderToolbar();
	void renderSceneView();
	void renderTransformGizmos();

	void updateMovement(Camera& camera); // Update movement within scene view
	glm::vec2 checkCursorBoundaries(glm::vec2 min, glm::vec2 max, bool& positionedCursor);

private:
	glm::vec2 lastContentRegionAvail; // Cache for last frames content region avail

	bool windowFocused; // Set if scene window is currently focused
	bool windowHovered; // Set if scene window is currently hovered

	bool sceneViewRightclicked; // Set if scene viewport is currently rightclicked
	bool sceneViewMiddleclicked; // Set if scene viewport is currently rightclicked

	float movementSpeed; // Speed of camera movement within scene viewport
	float mouseSensitivity; // Scene viewport mouse sensitivity
	float scrollIncrementSpeed; // Speed for incrementing movement speed through mouse scrolling

	glm::vec2 moveAxis; // Current input axis for evaluating scene view movement
	float moveAxisSmoothingFactor; // Smoothing for key axis
	glm::vec2 cursorCurrent; // Current mouse position
	glm::vec2 cursorLast; // Last mouse position
	glm::vec2 cursorDelta; // Last mouse position

	int gizmoOperation; // Last cached gizmo operation
};