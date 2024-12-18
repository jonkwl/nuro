#include "ui_components.h"

#include <imgui.h>
#include "../src/ui/editor_ui.h"
#include "../src/ui/ui_layout.h"
#include "../src/ui/ui_utils.h"

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
	ImGui::PushFont(EditorUI::getFonts().uiHeadline);
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

bool UIComponents::buttonBig(std::string label, std::string tooltip)
{
	bool pressed = false;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

	ImGui::PushStyleColor(ImGuiCol_ButtonActive, UIUtils::darken(EditorUI::getColors().elementActive, 0.3f));
	
	pressed = ImGui::Button(label.c_str());

	UIComponents::tooltip(tooltip);

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();

	return pressed;
}

void UIComponents::toggleButton(std::string label, bool& value, std::string tooltip)
{
	ImVec4 inactiveButtonColor = UIUtils::darken(EditorUI::getColors().elementActive, 0.6f);
	ImVec4 activeButtonColor = EditorUI::getColors().elementActive;
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

void UIComponents::label(std::string text, glm::vec4 color)
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
	label(text);
	ImGui::PopStyleColor();
}

void UIComponents::labelBold(std::string text)
{
	ImGui::PushFont(EditorUI::getFonts().uiBold);
	ImGui::Text(text.c_str());
	ImGui::PopFont();
}

void UIComponents::labelBold(std::string text, glm::vec4 color)
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
	labelBold(text);
	ImGui::PopStyleColor();
}

void UIComponents::labelSmall(std::string text)
{
	ImGui::PushFont(EditorUI::getFonts().uiSmall);
	ImGui::Text(text.c_str());
	ImGui::PopFont();
}

void UIComponents::labelSmall(std::string text, glm::vec4 color)
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
	labelSmall(text);
	ImGui::PopStyleColor();
}

void UIComponents::tryIcon(const char* icon, float yPadding)
{
	if (icon && icon[0] != '\0')
	{
		float currentY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(currentY + yPadding);
		ImGui::Text(icon);
		ImGui::SetCursorPosY(currentY);
	}
}

void UIComponents::tryIcon(const char* icon, glm::vec4 color, float yPadding)
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
	tryIcon(icon, yPadding);
	ImGui::PopStyleColor();
}

void UIComponents::input(std::string label, bool& value)
{
	UILayout::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
	ImGui::Text(label.c_str());
	ImGui::Spring(1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::Checkbox(EditorUI::generateId().c_str(), &value);
	ImGui::PopStyleVar();
	UILayout::endFlex();
}

void UIComponents::input(std::string label, int& value, float speed)
{
	UILayout::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
	ImGui::Text(label.c_str());
	ImGui::Spring(1.0f);
	ImGui::DragInt(EditorUI::generateId().c_str(), &value, speed);
	UILayout::endFlex();
}

void UIComponents::input(std::string label, float& value, float speed)
{
	UILayout::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
	ImGui::Text(label.c_str());
	ImGui::Spring(1.0f);
	ImGui::DragFloat(EditorUI::generateId().c_str(), &value, speed);
	UILayout::endFlex();
	// ImGui::DragFloat(std::string(label + EditorUI::generateId()).c_str(), &value, speed);
}

void UIComponents::indicatorLabel(std::string label, std::string value, std::string additional)
{
	ImGui::Text(label.c_str());
	ImGui::SameLine();
	ImGui::PushFont(EditorUI::getFonts().uiBold);
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
	UILayout::beginFlex(EditorUI::generateId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 20.0f, Justification::START, Alignment::CENTER, 0.0f, Margin(2.5f, 0.0f, 0.0f, 0.0f));

	ImGui::Checkbox(EditorUI::generateId().c_str(), &value);
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
	return ImGui::CollapsingHeader(std::string(label + EditorUI::generateId()).c_str());
}

void UIComponents::colorPicker(std::string label, float value[3])
{
	if (ImGui::CollapsingHeader(std::string(label + EditorUI::generateId()).c_str()))
	{
		ImGui::PushItemWidth(140);
		ImGui::ColorPicker3(EditorUI::generateId().c_str(), value, ImGuiColorEditFlags_NoSidePreview);
		ImGui::PopItemWidth();
	}
}
