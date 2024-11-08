#include "scene_view.h"

void SceneView::prepare()
{
	ImGui::Begin("  Scene View  ", nullptr, EngineUI::windowFlags.standard);

        float windowWidth = ImGui::GetWindowWidth();
        float contentWidth = ImGui::GetContentRegionAvail().x;
        float rightPadding = windowWidth - contentWidth;

        ImVec2 layout_size = ImVec2(windowWidth - rightPadding, 45.0f);

        static ImVec2 item_spacing = ImVec2(10.0f, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(floorf(item_spacing.x), floorf(item_spacing.y)));
        ImGui::PushID(1);
        ImGui::BeginHorizontal("h1", layout_size, 0.5f);
            ImGui::Spring(0.5f);

            float currentY = ImGui::GetCursorPosY();
            float padding = 2.5f;
            ImGui::SetCursorPosY(currentY + padding);
            ImGui::Text(ICON_FA_MAP);
            ImGui::SetCursorPosY(currentY);

            ImGui::PushFont(EngineUI::fonts.uiHeadline);
            ImGui::Text("Scene View");
            ImGui::PopFont();

            ImGui::Spring(0.5f);
        ImGui::EndHorizontal();
        ImGui::PopID();
        ImGui::PopStyleVar();

        ImGui::PushID(2);
        ImGui::BeginHorizontal("h1", layout_size, 0.5f);
            ImGui::Spring(0.5f);

            UIComponents::toggleButton(Runtime::wireframe, ICON_FA_VECTOR_SQUARE, 0, "Wireframe");
            UIComponents::toggleButton(Runtime::solidMode, ICON_FA_CUBE, 1, "Solid Mode");
            UIComponents::toggleButton(Runtime::shadows, ICON_FA_ECLIPSE, 2, "Shadows");
            UIComponents::toggleButton(Runtime::postProcessingEffects, ICON_FA_SPARKLES, 3, "Post Processing");

            ImGui::Spring(0.5f);
        ImGui::EndHorizontal();
        ImGui::PopID();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_Border));

	ImGui::End();
}
