#pragma once

#include <string>
#include <glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class EngineDialog
{
public:
	static void vec3_dialog(std::string name, glm::vec3& value, float min = -10.0f, float max = 10.0f);
	static void float_dialog(std::string name, float& value, float min = -10.0f, float max = 10.0f);
	static void bool_dialog(std::string name, bool& value);
	static void color_dialog(std::string name, glm::vec4& value);
};

