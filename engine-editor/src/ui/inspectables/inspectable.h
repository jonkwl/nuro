#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <string>

class Inspectable
{
public:
	virtual void renderStaticContent(ImDrawList& drawList) = 0;
	virtual void renderDynamicContent(ImDrawList& drawList) = 0;
};
