#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "../engine/window/window.h"

class EngineUI
{
public:
	static void setup();
	static void newFrame();
	static void render();
};