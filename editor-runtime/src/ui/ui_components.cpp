#include "ui_components.h"

#include <implot.h>
#include <algorithm>

#include "../src/ui/editor_ui.h"
#include "../src/ui/ui_flex.h"
#include "../src/ui/ui_utils.h"

namespace UIComponents {

	std::string _formatInteger(int32_t number)
	{
		std::string numStr = std::to_string(number);
		int32_t insertPosition = numStr.length() - 3;

		while (insertPosition > 0)
		{
			numStr.insert(insertPosition, ".");
			insertPosition -= 3;
		}

		return numStr;
	}

	void headline(std::string title, const char* icon, HeadlineJustification justification, bool seperator)
	{
		const float marginTop = 20.0f - ImGui::GetStyle().WindowPadding.y;
		const float marginBottom = 3.0f;
		const float marginSubSeperator = 7.0f;

		ImGui::Dummy(ImVec2(0.0f, marginTop));
		UIFlex::beginFlex(title.c_str(), FlexType::ROW, FLEX_FULL_WIDTH, 20.0f, (Justification)justification, Alignment::CENTER, 10.0f, Margin());
		{
			tryIcon(icon);
			ImGui::PushFont(EditorUI::getFonts().h4_bold);
			ImGui::Text(title.c_str());
			ImGui::PopFont();
		}
		UIFlex::endFlex();
		ImGui::Dummy(ImVec2(0.0f, marginBottom));
		if (seperator) {
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, marginSubSeperator));
		}
	}

	void tooltip(std::string tooltip)
	{
		if (!tooltip.empty())
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));

				ImGui::SetTooltip("%s", tooltip.c_str());

				ImGui::PopStyleVar();
				ImGui::PopStyleColor(2);
			}
		}
	}

	void toggleButton(std::string label, bool& value, std::string _tooltip)
	{
		ImU32 inactiveButtonColor = UIUtils::darken(EditorColor::elementActive, 0.6f);
		ImU32 activeButtonColor = EditorColor::elementActive;
		ImU32 currentButtonColor = value ? activeButtonColor : inactiveButtonColor;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, currentButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentButtonColor);

		ImGui::PushID(label.c_str());

		if (ImGui::Button(label.c_str()))
		{
			value = !value;
		}

		tooltip(_tooltip);

		ImGui::PopID();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(4);
	}

	bool buttonBig(std::string label, std::string _tooltip)
	{
		bool pressed = false;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UIUtils::darken(EditorColor::elementActive, 0.3f));

		pressed = ImGui::Button(label.c_str());

		tooltip(_tooltip);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		return pressed;
	}

	bool buttonBig(std::string label, ImU32 color, std::string _tooltip)
	{
		bool pressed = false;
		
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UIUtils::lighten(color, 0.35f));
		
		pressed = buttonBig(label, _tooltip);
		
		ImGui::PopStyleColor(2);
		
		return pressed;
	}

	void label(std::string text)
	{
		ImGui::Text(text.c_str());
	}

	void label(std::string text, ImU32 color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text(text.c_str());
		ImGui::PopStyleColor();
	}

	void label(std::string text, ImFont* font)
	{
		ImGui::PushFont(font);
		ImGui::Text(text.c_str());
		ImGui::PopFont();
	}

	void label(std::string text, ImFont* font, ImU32 color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(font);
		ImGui::Text(text.c_str());
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}

	void labelBold(std::string text)
	{
		ImGui::PushFont(EditorUI::getFonts().p_bold);
		ImGui::Text(text.c_str());
		ImGui::PopFont();
	}

	void labelBold(std::string text, ImU32 color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		labelBold(text);
		ImGui::PopStyleColor();
	}

	void labelHeadlineSized(std::string text)
	{
		ImGui::PushFont(EditorUI::getFonts().h4);
		ImGui::Text(text.c_str());
		ImGui::PopFont();
	}

	void labelHeadlineSized(std::string text, ImU32 color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		labelHeadlineSized(text);
		ImGui::PopStyleColor();
	}

	void labelSmall(std::string text)
	{
		ImGui::PushFont(EditorUI::getFonts().s);
		ImGui::Text(text.c_str());
		ImGui::PopFont();
	}

	void labelSmall(std::string text, ImU32 color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		labelSmall(text);
		ImGui::PopStyleColor();
	}

	void labelBig(std::string text)
	{
		ImGui::PushFont(EditorUI::getFonts().h2);
		ImGui::Text(text.c_str());
		ImGui::PopFont();
	}

	void labelBig(std::string text, ImU32 color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		labelBig(text);
		ImGui::PopStyleColor();
	}

	void tryIcon(const char* icon, float yPadding)
	{
		if (icon && icon[0] != '\0')
		{
			float currentY = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(currentY + yPadding);
			ImGui::Text(icon);
			ImGui::SetCursorPosY(currentY);
		}
	}

	void tryIcon(const char* icon, ImU32 color, float yPadding)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		tryIcon(icon, yPadding);
		ImGui::PopStyleColor();
	}

	void input(std::string label, bool& value)
	{
		UIFlex::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, FLEX_FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
		ImGui::Text(label.c_str());
		ImGui::Spring(1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox(EditorUI::generateId().c_str(), &value);
		ImGui::PopStyleVar();
		UIFlex::endFlex();
	}

	void input(std::string label, int32_t& value, float speed)
	{
		UIFlex::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, FLEX_FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
		ImGui::Text(label.c_str());
		ImGui::Spring(1.0f);
		ImGui::DragInt(EditorUI::generateId().c_str(), &value, speed);
		UIFlex::endFlex();
	}

	void input(std::string label, float& value, float speed)
	{
		UIFlex::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, FLEX_FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
		ImGui::Text(label.c_str());
		ImGui::Spring(1.0f);
		ImGui::DragFloat(EditorUI::generateId().c_str(), &value, speed);
		UIFlex::endFlex();
		// ImGui::DragFloat(std::string(label + EditorUI::generateId()).c_str(), &value, speed);
	}

	void indicatorLabel(std::string label, std::string value, std::string additional)
	{
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::PushFont(EditorUI::getFonts().p_bold);
		ImGui::Text(value.c_str());
		ImGui::PopFont();
		if (!additional.empty())
		{
			ImGui::SameLine();
			ImGui::Text(additional.c_str());
		}
	}

	void indicatorLabel(std::string label, int32_t value, std::string additional)
	{
		indicatorLabel(label, _formatInteger(value).c_str(), additional);
	}

	void indicatorLabel(std::string label, uint32_t value, std::string additional)
	{
		indicatorLabel(label, _formatInteger(value).c_str(), additional);
	}

	void indicatorLabel(std::string label, float value, std::string additional)
	{
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%.0f", value);
		indicatorLabel(label, buffer, additional);
	}

	void indicatorLabel(std::string label, double value, std::string additional)
	{
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%.0f", value);
		if (buffer[strlen(buffer) - 1] == '0' && buffer[strlen(buffer) - 2] == '.')
		{
			buffer[strlen(buffer) - 2] = '\0';
		}
		indicatorLabel(label, buffer, additional);
	}

	bool extendableSettings(std::string label, bool& value, const char* icon)
	{
		UIFlex::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, FLEX_FULL_WIDTH, 20.0f, Justification::START, Alignment::CENTER, 0.0f, Margin(2.5f, 0.0f, 0.0f, 0.0f));

		ImGui::Checkbox(EditorUI::generateId().c_str(), &value);
		ImGui::Dummy(ImVec2(1.0f, 1.5f));
		tryIcon(icon);

		if (value)
		{

			ImGui::Dummy(ImVec2(4.0f, 2.5f));
			bool extended = ImGui::CollapsingHeader(label.c_str());
			UIFlex::endFlex();

			if (extended)
			{
				ImGui::Dummy(ImVec2(0.0f, 8.0f));
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{

			ImGui::Text(label.c_str());
		}

		UIFlex::endFlex();

		return false;
	}

	bool header(std::string label)
	{
		return ImGui::CollapsingHeader(std::string(label + EditorUI::generateId()).c_str());
	}

	void colorPicker(std::string label, float value[3])
	{
		if (ImGui::CollapsingHeader(std::string(label + EditorUI::generateId()).c_str()))
		{
			ImGui::PushItemWidth(140);
			ImGui::ColorPicker3(EditorUI::generateId().c_str(), value, ImGuiColorEditFlags_NoSidePreview);
			ImGui::PopItemWidth();
		}
	}

	void sparklineGraph(const char* id, const float* values, int32_t count, float min_v, float max_v, int32_t offset, const ImVec4& color, const ImVec2& size)
	{
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
		if (ImPlot::BeginPlot(id, size, ImPlotFlags_CanvasOnly))
		{
			ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
			ImPlot::SetNextLineStyle(color);
			ImPlot::SetNextFillStyle(color, 0.25);
			ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
			ImPlot::EndPlot();
		}
		ImPlot::PopStyleVar();
	}

}