#include "ui_components.h"

#include <imgui.h>
#include "../src/editor/editor_ui.h"
#include "../src/editor/ui_layout.h"

std::string formatInteger(int number)
{
	std::string numStr = std::to_string(number);
	int insertPosition = numStr.length() - 3;

	while (insertPosition > 0)
	{
		numStr.insert(insertPosition, ".");
		insertPosition -= 3;
	}

	return numStr;
}

void UIComponents::headline(std::string title, const char* icon, HeadlineJustification justification, bool zeroMargin)
{
	Margin margin = zeroMargin ? Margin() : Margin(0.0f, 0.0f, 5.0f, 0.0f);
	UILayout::beginFlex(title.c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 20.0f, (Justification)justification, Alignment::CENTER, 10.0f, margin);

	tryIcon(icon);
	ImGui::PushFont(EditorUI::fonts.uiHeadline);
	ImGui::Text(title.c_str());
	ImGui::PopFont();

	UILayout::endFlex();
}

void UIComponents::tooltip(std::string tooltip)
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

void UIComponents::toggleButton(std::string label, bool& value, std::string tooltip)
{
	ImVec4 inactiveButtonColor = EditorUI::darken(EditorUI::colors.elementActive, 0.6f);
	ImVec4 activeButtonColor = EditorUI::colors.elementActive;
	ImVec4 currentButtonColor = value ? activeButtonColor : inactiveButtonColor;

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

	UIComponents::tooltip(tooltip);

	ImGui::PopID();
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(4);
}

void UIComponents::label(std::string text)
{
	ImGui::Text(text.c_str());
}

void UIComponents::labelBold(std::string text)
{
	ImGui::PushFont(EditorUI::fonts.uiBold);
	ImGui::Text(text.c_str());
	ImGui::PopFont();
}

void UIComponents::tryIcon(const char* icon, float padding)
{
	if (icon && icon[0] != '\0')
	{
		float currentY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(currentY + padding);
		ImGui::Text(icon);
		ImGui::SetCursorPosY(currentY);
	}
}

void UIComponents::input(std::string label, bool& value)
{
	UILayout::beginFlex(EditorUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
	ImGui::Text(label.c_str());
	ImGui::Spring(1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::Checkbox(EditorUI::getId().c_str(), &value);
	ImGui::PopStyleVar();
	UILayout::endFlex();
}

void UIComponents::input(std::string label, int& value, float speed)
{
	UILayout::beginFlex(EditorUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
	ImGui::Text(label.c_str());
	ImGui::Spring(1.0f);
	ImGui::DragInt(EditorUI::getId().c_str(), &value, speed);
	UILayout::endFlex();
}

void UIComponents::input(std::string label, float& value, float speed)
{
	UILayout::beginFlex(EditorUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
	ImGui::Text(label.c_str());
	ImGui::Spring(1.0f);
	ImGui::DragFloat(EditorUI::getId().c_str(), &value, speed);
	UILayout::endFlex();
	// ImGui::DragFloat(std::string(label + EditorUI::getId()).c_str(), &value, speed);
}

void UIComponents::indicatorLabel(std::string label, std::string value, std::string additional)
{
	ImGui::Text(label.c_str());
	ImGui::SameLine();
	ImGui::PushFont(EditorUI::fonts.uiBold);
	ImGui::Text(value.c_str());
	ImGui::PopFont();
	if (!additional.empty())
	{
		ImGui::SameLine();
		ImGui::Text(additional.c_str());
	}
}

void UIComponents::indicatorLabel(std::string label, int value, std::string additional)
{
	indicatorLabel(label, formatInteger(value).c_str(), additional);
}

void UIComponents::indicatorLabel(std::string label, unsigned int value, std::string additional)
{
	indicatorLabel(label, formatInteger(value).c_str(), additional);
}

void UIComponents::indicatorLabel(std::string label, float value, std::string additional)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "%.0f", value);
	indicatorLabel(label, buffer, additional);
}

void UIComponents::indicatorLabel(std::string label, double value, std::string additional)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "%.2f", value);
	if (buffer[strlen(buffer) - 1] == '0' && buffer[strlen(buffer) - 2] == '.')
	{
		buffer[strlen(buffer) - 2] = '\0';
	}
	indicatorLabel(label, buffer, additional);
}

bool UIComponents::extendableSettings(std::string label, bool& value, const char* icon)
{
	UILayout::beginFlex(EditorUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 20.0f, Justification::START, Alignment::CENTER, 0.0f, Margin(2.5f, 0.0f, 0.0f, 0.0f));

	ImGui::Checkbox(EditorUI::getId().c_str(), &value);
	UIComponents::space(1.0f, 1.5f);
	tryIcon(icon);

	if (value)
	{

		UIComponents::space(4.0f, 2.5f);
		bool extended = ImGui::CollapsingHeader(label.c_str());
		UILayout::endFlex();

		if (extended)
		{
			UIComponents::space(0.0f, 8.0f);
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

	UILayout::endFlex();

	return false;
}

void UIComponents::space(float width, float height)
{
	ImGui::Dummy(ImVec2(width, height));
}

bool UIComponents::header(std::string label)
{
	return ImGui::CollapsingHeader(std::string(label + EditorUI::getId()).c_str());
}

void UIComponents::colorPicker(std::string label, float value[3])
{
	if (ImGui::CollapsingHeader(std::string(label + EditorUI::getId()).c_str()))
	{
		ImGui::PushItemWidth(140);
		ImGui::ColorPicker3(EditorUI::getId().c_str(), value, ImGuiColorEditFlags_NoSidePreview);
		ImGui::PopItemWidth();
	}
}
