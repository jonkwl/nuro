#pragma once

#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "../engine/engineui/engine_ui.h"

#include <iostream>

struct InputPair {
	std::string name;

	float& floatValue;

	float sliderMin;
	float sliderMax;
};

class EngineDialog
{
public:
	static void vec3_dialog(std::string name, glm::vec3& value, float min = -10.0f, float max = 10.0f);
	static void float_dialog(std::string name, float& value, float min = -10.0f, float max = 10.0f);
	static void bool_dialog(std::string name, bool& value);
	static void color_dialog(std::string name, glm::vec4& value);

	static void input_dialog(std::string name, std::vector<InputPair> inputs);

	static void plot_demo();
	
	static void show_diagnostics(float deltaTime, int fps, float averageFps);
private:
	static std::deque<int> fpsCache;
	static float fpsUpdateTimer;
};

