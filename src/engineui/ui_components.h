#pragma once

#include <string>

enum HeadlineJustification {
	HEADLINE_LEFT,
	HEADLINE_CENTER,
	HEADLINE_RIGHT
};

class UIComponents
{
public:
	static void headline(std::string title, const char* icon = "", HeadlineJustification justification = HEADLINE_LEFT, bool zeroMargin = false);
	static void tooltip(const char* tooltip);
	static void toggleButton(const char* label, bool& value, const char* tooltip = "");

	static void label(std::string text);
	static void labelBold(std::string text);

	static void tryIcon(const char* icon, float padding = 2.5f);
	
	static void input(const char* label, bool& value);
	static void input(const char* label, int& value, float speed = 0.1f);
	static void input(const char* label, float& value, float speed = 0.1f);

	static void indicatorLabel(const char* label, const char* value, const char* additional = "");
	static void indicatorLabel(const char* label, int value, const char* additional = "");
	static void indicatorLabel(const char* label, unsigned int value, const char* additional = "");
	static void indicatorLabel(const char* label, float value, const char* additional = "");
	static void indicatorLabel(const char* label, double value, const char* additional = "");

	static bool extendableSettings(const char* label, bool& value, const char* icon = "");

	static void space(float width, float height);

	static bool header(const char* label);

	static void colorPicker(const char* label, float value[3]);
};