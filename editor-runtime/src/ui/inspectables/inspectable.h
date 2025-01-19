#pragma once

#include <string>
#include <imgui.h>

class Inspectable
{
public:
	virtual std::string getName() { 
		return "Default";
	}

	virtual void renderStaticContent(ImDrawList& drawList) { }
	virtual void renderDynamicContent(ImDrawList& drawList) { }
};
