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

	// Keeps the cursor in the given boundaries, returns the new cursor position and sets if the cursor was positioned by method
	glm::vec2 keepCursorInBounds(glm::vec4 bounds, bool& positionedCursor);

private:
	glm::vec2 lastContentRegionAvail; // Cache for last frames content region avail

	bool windowFocused; // Set if scene window is currently focused
	bool windowHovered; // Set if scene window is currently hovered

	bool sceneViewRightclicked; // Set if scene viewport is currently rightclicked
	bool sceneViewMiddleclicked; // Set if scene viewport is currently rightclicked

	glm::vec4 sceneViewBounds; // Bounds of scene view (min x, min y, max x, max y)

	float movementSpeed; // Speed of camera movement within scene viewport
	float mouseSensitivity; // Scene viewport mouse sensitivity
	float scrollIncrementSpeed; // Speed for incrementing movement speed through mouse scrolling

	glm::vec2 moveAxis; // Current input axis for evaluating scene view movement
	float moveAxisSmoothingFactor; // Smoothing for key axis
	glm::vec2 cursorCurrent; // Current mouse position
	glm::vec2 cursorLast; // Last mouse position
	glm::vec2 cursorDelta; // Last mouse position

	int gizmoOperation; // Last used gizmo operation
	float gizmoScaleMin; // Limit for downscaling an object with a gizmo
};