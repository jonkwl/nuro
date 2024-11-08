#include "scene_view.h"

void SceneView::prepare()
{
	ImGui::Begin("Scene View", nullptr, EngineUI::windowFlags.fixed);

	EngineUI::headline("Scene View");

    ImVec4 inactiveButtonColor = EngineUI::colors.elementActive;
    ImVec4 activeButtonColor = EngineUI::darken(inactiveButtonColor, 0.5f);
    ImVec4 currentButtonColor = wireframe ? activeButtonColor : inactiveButtonColor;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));

    ImGui::PushStyleColor(ImGuiCol_Button, currentButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentButtonColor);

    if (ImGui::Button(ICON_FA_ARCHWAY))
    {
        wireframe = !wireframe;
    }

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(3);

	ImGui::End();
}
