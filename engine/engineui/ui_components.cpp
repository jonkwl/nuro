#include "ui_components.h"

#include "../engineui/engine_ui.h"

std::string getId(const char* name) {
    return "##" + std::string(name);
}

void UIComponents::headline(std::string title, const char* icon, HeadlineAlignment alignment) {

    UILayout::beginRow("example", FULL_WIDTH, 20.0f, (ItemAlignment)alignment, 0.5f, 10.0f);

        // Icon
        if (icon && icon[0] != '\0') {
            float currentY = ImGui::GetCursorPosY();
            float padding = 2.5f;
            ImGui::SetCursorPosY(currentY + padding);
            ImGui::Text(icon);
            ImGui::SetCursorPosY(currentY);
        }

        // Label
        ImGui::PushFont(EngineUI::fonts.uiHeadline);
        ImGui::Text(title.c_str());
        ImGui::PopFont();

    UILayout::endRow();

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

void UIComponents::input(const char* label, int& value)
{
    UILayout::beginRow(label, FULL_WIDTH, 30.0f, ITEMS_SPREAD, 0.5f, 4.0f, false);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::InputInt(getId(label).c_str(), &value);
    UILayout::endRow();
}

void UIComponents::input(const char* label, float& value)
{
    UILayout::beginRow(label, FULL_WIDTH, 30.0f, ITEMS_SPREAD, 0.5f, 4.0f, false);
        ImGui::Text(label);
        ImGui::Spring(1.0f);
        ImGui::InputFloat(getId(label).c_str(), &value);
    UILayout::endRow();
}