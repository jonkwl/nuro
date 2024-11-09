#include "ui_components.h"

#include "../engineui/engine_ui.h"

std::string getId(const char* name, const char* identifier) {
    return "##" + std::string(name) + "_" + std::string(identifier);
}

void UIComponents::headline(std::string title, const char* icon, HeadlineJustification justification, bool zeroMargin) {
    Margin margin = zeroMargin ? Margin() : Margin(0.0f, 0.0f, 5.0f, 0.0f);
    UILayout::beginFlex(title.c_str(), ROW, FULL_WIDTH, 20.0f, (Justification)justification, ALIGN_CENTER, 10.0f, margin);

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

void UIComponents::tryIcon(const char* icon)
{
    if (icon && icon[0] != '\0') {
        float currentY = ImGui::GetCursorPosY();
        float padding = 2.5f;
        ImGui::SetCursorPosY(currentY + padding);
        ImGui::Text(icon);
        ImGui::SetCursorPosY(currentY);
    }
}

void UIComponents::input(const char* label, int& value)
{
    UILayout::beginFlex(label, ROW, FULL_WIDTH, 30.0f, JUSTIFY_EVEN, ALIGN_CENTER, 4.0f);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::InputInt(getId(label, "input").c_str(), &value);
    UILayout::endFlex();
}

void UIComponents::input(const char* label, float& value)
{
    UILayout::beginFlex(label, ROW, FULL_WIDTH, 30.0f, JUSTIFY_EVEN, ALIGN_CENTER, 4.0f);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::InputFloat(getId(label, "input").c_str(), &value);
    UILayout::endFlex();
}

bool UIComponents::extendableSettings(const char* label, bool& value, const char* icon)
{
    UILayout::beginFlex(getId(label, "flex").c_str(), ROW, FULL_WIDTH, 20.0f, JUSTIFY_START, ALIGN_CENTER, 5.0f, Margin(10.0f, 0.0f, 5.0f, 0.0f));
    
        ImGui::Checkbox(getId(label, "enable").c_str(), &value);
        if (value) {

            ImGui::Dummy(ImVec2(4.0f, 0.0f));
            bool extended = ImGui::CollapsingHeader(label);
            UILayout::endFlex();

            if (extended) {
                UIComponents::headline((std::string(label) + " Settings:").c_str(), ICON_FA_PROJECTOR, HEADLINE_LEFT, true);
                return true;
            }
            else {
                return false;
            }

        }
        else {

            ImGui::Dummy(ImVec2(1.0f, 0.0f));
            tryIcon(icon);
            ImGui::PushFont(EngineUI::fonts.uiHeadline);
            ImGui::Text(label);
            ImGui::PopFont();

        }

    UILayout::endFlex();
    
    return false;
}