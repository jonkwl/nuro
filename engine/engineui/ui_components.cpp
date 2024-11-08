#include "ui_components.h"

#include "../engineui/engine_ui.h"

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

void UIComponents::toggleButton(bool& value, const char* label, unsigned int id, const char* tooltip)
{
    ImVec4 inactiveButtonColor = EngineUI::colors.elementActive;
    ImVec4 activeButtonColor = EngineUI::darken(inactiveButtonColor, 0.6f);
    ImVec4 currentButtonColor = value ? activeButtonColor : inactiveButtonColor;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, currentButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentButtonColor);

    ImGui::PushID(id);

    if (ImGui::Button(label))
    {
        value = !value;
    }

    UIComponents::tooltip(tooltip);

    ImGui::PopID();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(4);
}