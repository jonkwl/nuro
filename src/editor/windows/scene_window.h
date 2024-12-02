#pragma once

#include <glm.hpp>

#include "../src/editor/editor_window.h"
#include "../src/camera/camera.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow();
	void render();

private:
	void renderToolbar();
	void renderSceneView();

	void updateMovement(Camera& camera); // Update movement within scene view
	glm::vec2 checkCursorBoundaries(glm::vec2 min, glm::vec2 max, bool& positionedCursor);

private:
	glm::vec2 lastContentRegionAvail; // Cache for last frames content region avail

	bool sceneViewRightclicked; // Set if scene viewport is currently rightclicked

	float movementSpeed; // Speed of camera movement within scene viewport
	float mouseSensitivity; // Scene viewport mouse sensitivity
	float scrollIncrementSpeed; // Speed for incrementing movement speed through mouse scrolling

	glm::vec2 keyAxis; // Current input axis for evaluating scene view movement
	float keyAxisSmoothingFactor; // Smoothing for key axis
	glm::vec2 mouseCurrent; // Current mouse position
	glm::vec2 mouseLast; // Last mouse position
	glm::vec2 mouseAxis; // Last mouse position
};