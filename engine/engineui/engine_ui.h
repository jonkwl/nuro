#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../engine/context/context.h"

class EngineUI
{
public:
	static void Setup();
	static void NewFrame();
	static void Render();
};