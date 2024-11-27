#pragma once

#include <string>

enum class HeadlineJustification
{
	LEFT,
	CENTER,
	RIGHT
};

class UIComponents
{
public:
	static void headline(std::string title, const char *icon = "", HeadlineJustification justification = HeadlineJustification::LEFT, bool zeroMargin = false);
	static void tooltip(std::string tooltip);
	static void toggleButton(std::string label, bool &value, std::string tooltip = "");

	static void label(std::string text);
	static void labelBold(std::string text);

	static void tryIcon(const char *icon, float padding = 2.5f);

	static void input(std::string label, bool &value);
	static void input(std::string label, int &value, float speed = 0.1f);
	static void input(std::string label, float &value, float speed = 0.1f);

	static void indicatorLabel(std::string label, std::string value, std::string additional = "");
	static void indicatorLabel(std::string label, int value, std::string additional = "");
	static void indicatorLabel(std::string label, unsigned int value, std::string additional = "");
	static void indicatorLabel(std::string label, float value, std::string additional = "");
	static void indicatorLabel(std::string label, double value, std::string additional = "");

	static bool extendableSettings(std::string label, bool &value, const char *icon = "");

	static void space(float width, float height);

	static bool header(std::string label);

	static void colorPicker(std::string label, float value[3]);
};