#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Context
{
public:
	// Window
	int x = 0;
	int y = 0;
	int width = 800;
	int height = 800;
	string title = "Rendering Engine";
	bool fullscreen = true;
	GLFWwindow* window = nullptr;

	// Render settings
	bool wireframe = false;

	// Time
	float time = 0.0;
	float last_time = 0.0;
	float delta_time = 0.0;

	// Environmental settings
	glm::vec4 background_color;

	// Cursor settings
	GLenum cursor_mode = GLFW_CURSOR_DISABLED;
	glm::vec2 mouse_last;

	// Camera settings
	float camera_fov = 70;
	glm::vec3 camera_position;
	glm::vec3 camera_rotation;

	// Movement settings
	float movement_speed = 12.0f;
	float sensitivity = 6.0f;
	bool lock_movement = false;
	bool lock_lookaround = false;

	// Input settings
	float key_smooth_factor = 5.0f;
	glm::vec2 key_axis;
	glm::vec2 key_axis_smooth;
	glm::vec2 mouse_axis;

	// Input cache
	bool esc_pressed = false;
	bool show_ui = false;
};

