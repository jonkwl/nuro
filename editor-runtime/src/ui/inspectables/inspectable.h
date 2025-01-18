#pragma once

#include <string>
#include <imgui.h>

class Inspectable
{
public:
	virtual std::string getName() { 
		return "Default";
	}

	virtual void render(ImDrawList& drawList) { }
};
