#pragma once

#include <glm.hpp>

#include "../src/editor/editor_window.h"
#include "../src/camera/camera.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow();
	void prepare();

private:
	void updateMovement(Camera& camera); // Update movement within scene view

private:
	glm::vec2 lastContentRegionAvail; // Cache for last frames content region avail

	bool sceneViewRightclicked; // Set if scene viewport is currently rightclicked

	float movementSpeed; // Speed of camera movement within scene viewport
	float mouseSensitivity; // Scene viewport mouse sensitivity
	float scrollIncrementSpeed; // Speed for incrementing movement speed through mouse scrolling

	glm::vec2 keyAxis; // Current input axis for evaluating scene view movement
	float keyAxisSmoothingFactor; // Smoothing for key axis
	glm::vec2 mouseAxis; // Current mouse axis
};