#include "ui_components.h"

#include "../engineui/engine_ui.h"

std::string formatInteger(int number) {
    std::string numStr = std::to_string(number);
    int insertPosition = numStr.length() - 3;

    while (insertPosition > 0) {
        numStr.insert(insertPosition, ".");
        insertPosition -= 3;
    }

    return numStr;
}

void UIComponents::headline(std::string title, const char* icon, HeadlineJustification justification, bool zeroMargin) {
    Margin margin = zeroMargin ? Margin() : Margin(0.0f, 0.0f, 5.0f, 0.0f);
    UILayout::beginFlex(title.c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 20.0f, (Justification)justification, Alignment::CENTER, 10.0f, margin);

        tryIcon(icon);
        ImGui::PushFont(EngineUI::fonts.uiHeadline);
        ImGui::Text(title.c_str());
        ImGui::PopFont();

    UILayout::endFlex();

}

void UIComponents::tooltip(const char* tooltip)
{
    if (tooltip && tooltip[0] != '\0') {
        if (ImGui::IsItemHovered()) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));

            ImGui::SetTooltip("%s", tooltip);

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
        }
    }
}

void UIComponents::toggleButton(const char* label, bool& value, const char* tooltip)
{
    ImVec4 inactiveButtonColor = EngineUI::darken(EngineUI::colors.elementActive, 0.6f);
    ImVec4 activeButtonColor = EngineUI::colors.elementActive;
    ImVec4 currentButtonColor = value ? activeButtonColor : inactiveButtonColor;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, currentButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentButtonColor);

    ImGui::PushID(label);

    if (ImGui::Button(label))
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
    ImGui::PushFont(EngineUI::fonts.uiBold);
    ImGui::Text(text.c_str());
    ImGui::PopFont();
}

void UIComponents::tryIcon(const char* icon, float padding)
{
    if (icon && icon[0] != '\0') {
        float currentY = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(currentY + padding);
        ImGui::Text(icon);
        ImGui::SetCursorPosY(currentY);
    }
}

void UIComponents::input(const char* label, bool& value)
{
    UILayout::beginFlex(EngineUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox(EngineUI::getId().c_str(), &value);
        ImGui::PopStyleVar();
    UILayout::endFlex();
}

void UIComponents::input(const char* label, int& value, float speed)
{
    UILayout::beginFlex(EngineUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::DragInt(EngineUI::getId().c_str(), &value, speed);
    UILayout::endFlex();
}

void UIComponents::input(const char* label, float& value, float speed)
{
    UILayout::beginFlex(EngineUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 18.0f, Justification::EVEN, Alignment::CENTER, 4.0f);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::DragFloat(EngineUI::getId().c_str(), &value, speed);
    UILayout::endFlex();
    // ImGui::DragFloat(std::string(label + EngineUI::getId()).c_str(), &value, speed);
}

void UIComponents::indicatorLabel(const char* label, const char* value, const char* additional)
{
    ImGui::Text(label);
    ImGui::SameLine();
    ImGui::PushFont(EngineUI::fonts.uiBold);
    ImGui::Text(value);
    ImGui::PopFont();
    if (additional && additional[0] != '\0') {
        ImGui::SameLine();
        ImGui::Text(additional);
    }
}

void UIComponents::indicatorLabel(const char* label, int value, const char* additional)
{
    indicatorLabel(label, formatInteger(value).c_str(), additional);
}

void UIComponents::indicatorLabel(const char* label, unsigned int value, const char* additional)
{
    indicatorLabel(label, formatInteger(value).c_str(), additional);
}

void UIComponents::indicatorLabel(const char* label, float value, const char* additional)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.0f", value);
    indicatorLabel(label, buffer, additional);
}

void UIComponents::indicatorLabel(const char* label, double value, const char* additional)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    if (buffer[strlen(buffer) - 1] == '0' && buffer[strlen(buffer) - 2] == '.')
    {
        buffer[strlen(buffer) - 2] = '\0';
    }
    indicatorLabel(label, buffer, additional);
}

bool UIComponents::extendableSettings(const char* label, bool& value, const char* icon)
{
    UILayout::beginFlex(EngineUI::getId().c_str(), FlexType::ROW, UILayout::FULL_WIDTH, 20.0f, Justification::START, Alignment::CENTER, 0.0f, Margin(2.5f, 0.0f, 0.0f, 0.0f));
    
        ImGui::Checkbox(EngineUI::getId().c_str(), &value);
        UIComponents::space(1.0f, 1.5f);
        tryIcon(icon);

        if (value) {

            UIComponents::space(4.0f, 2.5f);
            bool extended = ImGui::CollapsingHeader(label);
            UILayout::endFlex();

            if (extended) {
                UIComponents::space(0.0f, 8.0f);
                return true;
            }
            else {
                return false;
            }

        }
        else {

            ImGui::Text(label);

        }

    UILayout::endFlex();
    
    return false;
}

void UIComponents::space(float width, float height)
{
    ImGui::Dummy(ImVec2(width, height));
}

bool UIComponents::header(const char* label)
{
    return ImGui::CollapsingHeader(std::string(label + EngineUI::getId()).c_str());
}

void UIComponents::colorPicker(const char* label, float value[3])
{
    if (ImGui::CollapsingHeader(std::string(label + EngineUI::getId()).c_str())) {
        ImGui::PushItemWidth(140);
        ImGui::ColorPicker3(EngineUI::getId().c_str(), value, ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopItemWidth();
    }
}
