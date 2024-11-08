#pragma once

class UIComponents
{
public:
	static void tooltip(const char* tooltip);
	static void toggleButton(bool& value, const char* label, unsigned int id, const char* tooltip = "");
};

