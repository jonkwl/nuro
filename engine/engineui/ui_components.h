#pragma once

#include <string>

enum HeadlineAlignment {
	HEADLINE_LEFT,
	HEADLINE_CENTERED,
	HEADLINE_RIGHT
};

class UIComponents
{
public:
	static void headline(std::string title, const char* icon = "", HeadlineAlignment alignment = HEADLINE_LEFT);
	static void tooltip(const char* tooltip);
	static void toggleButton(const char* label, bool& value, const char* tooltip = "");
	
	static void input(const char* label, int& value);
	static void input(const char* label, float& value);
};