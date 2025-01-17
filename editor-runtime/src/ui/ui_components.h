#pragma once

#include <string>
#include <imgui.h>

enum class HeadlineJustification
{
	LEFT,
	CENTER,
	RIGHT
};

namespace UIComponents
{
	void headline(std::string title, const char* icon = "", HeadlineJustification justification = HeadlineJustification::LEFT, bool seperator = true);
	
	void tooltip(std::string tooltip);
	void toggleButton(std::string label, bool& value, std::string tooltip = "");

	bool buttonBig(std::string label, std::string tooltip = "");
	bool buttonBig(std::string label, ImU32 color, std::string tooltip = "");

	void label(std::string text);
	void label(std::string text, ImU32 color);
	void label(std::string text, ImFont* font);
	void label(std::string text, ImFont* font, ImU32 color);

	void tryIcon(const char* icon, float yPadding = 2.5f);
	void tryIcon(const char* icon, ImU32 color, float yPadding = 2.5f);

	void input(std::string label, bool& value);
	void input(std::string label, int32_t& value, float speed = 0.1f);
	void input(std::string label, float& value, float speed = 0.1f);

	void indicatorLabel(std::string label, std::string value, std::string additional = "");
	void indicatorLabel(std::string label, int32_t value, std::string additional = "");
	void indicatorLabel(std::string label, uint32_t value, std::string additional = "");
	void indicatorLabel(std::string label, float value, std::string additional = "");
	void indicatorLabel(std::string label, double value, std::string additional = "");

	bool extendableSettings(std::string label, bool& value, const char* icon = "");

	bool header(std::string label);

	void colorPicker(std::string label, float value[3]);

	void sparklineGraph(const char* id, const float* values, int32_t count, float min_v, float max_v, int32_t offset, const ImVec4& color, const ImVec2& size);
};