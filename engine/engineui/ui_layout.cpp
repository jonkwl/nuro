#include "ui_layout.h"

#include "../engineui/engine_ui.h"

bool UILayout::debugMode = true;

float UILayout::defaultWidth = -1.0f;
float UILayout::defaultHeight = -1.0f;
Justification UILayout::defaultJustification = JUSTIFY_CENTER;
Alignment UILayout::defaultAlignment = ALIGN_CENTER;
float UILayout::defaultSpacing = 0.0f;
Margin UILayout::defaultMargin = Margin();

FlexBuffer UILayout::lastFlex;

float UILayout::mapAlignment(Alignment alignment) {
    return ((int)alignment - 1) / 2;
}

ImVec2 UILayout::getFlexRowSize(float width, float height) {
    float windowWidth = ImGui::GetWindowWidth();
    float contentWidth = ImGui::GetContentRegionAvail().x;
    float rightPadding = windowWidth - contentWidth;
    float fullWidth = windowWidth - rightPadding;
    ImVec2 layoutSize = ImVec2((int)width == FULL_WIDTH ? fullWidth : width, height);
    return layoutSize;
}

void UILayout::beginFlex(const char* name, FlexType type, float width, float height, Justification justification, Alignment alignment, float spacing, Margin margin)
{
    if (type == ROW) {
        static ImVec2 itemSpacing = ImVec2(spacing, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(floorf(itemSpacing.x), floorf(itemSpacing.y)));

        ImGui::Dummy(ImVec2(0.0f, margin.top));

        ImVec2 rowSize = getFlexRowSize(width, height);
        ImGui::BeginHorizontal(name, rowSize, mapAlignment(alignment));

        if(margin.left != 0) ImGui::Dummy(ImVec2(margin.left, 0.0f));

        if (justification == JUSTIFY_CENTER || justification == JUSTIFY_END) {
            ImGui::Spring(0.5f);
        }

        lastFlex.type = type;
        lastFlex.width = width;
        lastFlex.height = height;
        lastFlex.justification = justification;
        lastFlex.alignment = alignment;
        lastFlex.spacing = spacing;
        lastFlex.margin = margin;
    }
}

void UILayout::endFlex()
{
    if (lastFlex.type == ROW) {
        if(lastFlex.margin.right != 0) ImGui::Dummy(ImVec2(lastFlex.margin.right, 0.0f));

        if (lastFlex.justification == JUSTIFY_CENTER || lastFlex.justification == JUSTIFY_START) {
            ImGui::Spring(0.5f);
        }

        ImGui::EndHorizontal();
        ImGui::PopStyleVar();

        ImGui::Dummy(ImVec2(0.0f, lastFlex.margin.bottom));

        if (debugMode) {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_Border));
        }
    }
}
