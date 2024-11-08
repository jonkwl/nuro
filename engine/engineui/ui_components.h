#pragma once

#include <string>

class UIComponents
{
public:
	static void spacing();
	static void headline(std::string title, const char* icon = "");
	static void tooltip(const char* tooltip);
	static void toggleButton(bool& value, const char* label, unsigned int id, const char* tooltip = "");
};

