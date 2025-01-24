#include "im_components.h"

#include <implot.h>
#include <algorithm>
#include <array>

#include "../src/ui/editor_ui.h"
#include "../src/ui/ui_flex.h"
#include "../src/ui/ui_utils.h"

#include "../core/utils/log.h"
#include "../src/ui/IconsFontAwesome6.h"

namespace IMComponents {

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
		UIFlex::beginFlex(EditorUI::generateId(), FlexType::ROW, FLEX_FULL_WIDTH, 20.0f, (Justification)justification, Alignment::CENTER, 10.0f, Margin());
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

	void toggleButton(ImDrawList& drawList, std::string text, bool& value, std::string tooltip, ImVec2 position)
	{
		// Setup style
		static const ImVec2 padding = ImVec2(8.0f, 8.0f);
		static const float rounding = 5.0f;
		static ImFont* const font = EditorUI::getFonts().s;
		static const float fontSize = font->FontSize;

		// Evaluate
		ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, -1.0f, text.c_str());
		ImVec2 buttonSize = textSize + padding * 2;
		ImVec2 p0 = position;
		ImVec2 p1 = position + buttonSize;
		bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
		bool clicked = hovered && ImGui::IsMouseClicked(0);
		if (clicked) value = !value;

		// Draw background
		drawList.AddRectFilled(p0, p1, value ? EditorColor::elementActive : EditorColor::element, rounding);

		// Draw outline
		// drawList.AddRect(p0, p1, outlineColor, rounding, ImDrawFlags_RoundCornersAll, outlineThickness);

		// Draw text
		drawList.AddText(font, fontSize, p0 + padding, EditorColor::text, text.c_str());
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
		if (ImGui::BeginTable(EditorUI::generateIdString().c_str(), 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow();

			// Label
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label.c_str());

			// Input
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-1);
			ImGui::Checkbox(EditorUI::generateIdString().c_str(), &value);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}

	void input(std::string label, int32_t& value, float speed)
	{
		if (ImGui::BeginTable(EditorUI::generateIdString().c_str(), 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow();

			// Label
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label.c_str());

			// Input
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-1);
			ImGui::DragInt(EditorUI::generateIdString().c_str(), &value, speed);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}

	void input(std::string label, float& value, float speed)
	{
		if (ImGui::BeginTable(EditorUI::generateIdString().c_str(), 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow();

			// Label
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label.c_str());

			// Input
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-1);
			ImGui::DragFloat(EditorUI::generateIdString().c_str(), &value, speed);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
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
		UIFlex::beginFlex(EditorUI::generateId(), FlexType::ROW, FLEX_FULL_WIDTH, 20.0f, Justification::START, Alignment::CENTER, 0.0f, Margin(2.5f, 0.0f, 0.0f, 0.0f));

		ImGui::Checkbox(EditorUI::generateIdString().c_str(), &value);
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
		return ImGui::CollapsingHeader(label.c_str());
	}

	void colorPicker(std::string label, float value[3])
	{
		ImU32 color = IM_COL32(
			static_cast<unsigned char>(value[0] * 255.0f),
			static_cast<unsigned char>(value[1] * 255.0f),
			static_cast<unsigned char>(value[2] * 255.0f),
			255);

		if (ImGui::BeginTable(EditorUI::generateIdString().c_str(), 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow();

			// Label
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label.c_str());

			// Pick button
			ImGui::TableSetColumnIndex(1);
			if (ImGui::Button("Pick Color"))
			{
				ImGui::OpenPopup("Color_Picker_Popup", ImGuiPopupFlags_AnyPopupLevel);
			}

			// Color indicator
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
			ImGui::Button(EditorUI::generateIdString().c_str(), ImVec2(-1.0f, 0.0f));
			ImGui::PopStyleColor(3);

			ImGui::EndTable();
		}

		// Color picker popup
		if (ImGui::BeginPopup("Color_Picker_Popup"))
		{
			/*ImGui::PushItemWidth(140);
			ImGui::ColorPicker3(EditorUI::generateIdString().c_str(), value, ImGuiColorEditFlags_NoSidePreview);
			ImGui::PopItemWidth();*/

			ImGui::Text("Color Picker Popup");

			if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
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

	void beginChild(ImVec2 size, ImVec2 position)
	{
		ImGui::SetCursorScreenPos(position);
		ImGui::PushClipRect(ImVec2(position), ImVec2(position + size), true);
		ImGui::BeginChild(EditorUI::generateId(), size);
	}

	void endChild()
	{
		ImGui::EndChild();
		ImGui::PopClipRect();
	}

	bool caret(ImDrawList& drawList, ImVec2 position, ImVec2 offset, const char* icon, ImU32 color, ImU32 hoveredColor)
	{
		// Circle geometry
		float circleRadius = 9.0f;
		ImVec2 circlePosition = ImVec2(
			position.x + circleRadius + offset.x,
			position.y + circleRadius * 0.5f + offset.y);
		const ImVec2 mousePosition = ImGui::GetMousePos();

		// Fetch circle interactions
		float circleDistance =
			(mousePosition.x - circlePosition.x) *
			(mousePosition.x - circlePosition.x) +
			(mousePosition.y - circlePosition.y) *
			(mousePosition.y - circlePosition.y);

		bool circleHovered = circleDistance <= (circleRadius * circleRadius);
		bool circleClicked = ImGui::IsMouseClicked(0) && circleHovered;

		// Evaluate color
		ImU32 circleColor = circleHovered ? hoveredColor : color;

		// Draw circle
		drawList.AddCircleFilled(circlePosition, circleRadius, circleColor);

		// Draw icon
		drawList.AddText(EditorUI::getFonts().h4_bold, EditorSizing::h4_FontSize, position, EditorColor::text, icon);

		return circleClicked;
	}

}