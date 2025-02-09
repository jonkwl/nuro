#pragma once

#include <string>
#include <imgui.h>

class Inspectable
{
public:
	virtual std::string getName() = 0;
	virtual void renderStaticContent(ImDrawList& drawList) = 0;
	virtual void renderDynamicContent(ImDrawList& drawList) = 0;
};
