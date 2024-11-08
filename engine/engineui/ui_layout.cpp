#include "ui_layout.h"

#include "../engineui/engine_ui.h"

LayoutElementBuffer UILayout::lastLayoutElement;

ImVec2 UILayout::getRowSize(float width, float height) {
    float windowWidth = ImGui::GetWindowWidth();
    float contentWidth = ImGui::GetContentRegionAvail().x;
    float rightPadding = windowWidth - contentWidth;
    float fullWidth = windowWidth - rightPadding;
    ImVec2 layoutSize = ImVec2((int)width == FULL_WIDTH ? fullWidth : width, height);
    return layoutSize;
}

void UILayout::setLastLayoutElement(ItemAlignment itemAlignment, bool showBoundaries) {
    lastLayoutElement.itemAlignment = itemAlignment;
    lastLayoutElement.showBoundaries = showBoundaries;
}

void UILayout::beginRow(const char* name, float width, float height, ItemAlignment itemAlignment, float verticalAlignment, float spacing, bool showBoundaries)
{
    static ImVec2 itemSpacing = ImVec2(spacing, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(floorf(itemSpacing.x), floorf(itemSpacing.y)));

    ImVec2 rowSize = getRowSize(width, height);
    ImGui::BeginHorizontal(name, rowSize, verticalAlignment);

    if (itemAlignment == ITEMS_CENTERED ||  itemAlignment == ITEMS_RIGHT) {
        ImGui::Spring(0.5f);
    }

    setLastLayoutElement(itemAlignment, showBoundaries);
}

void UILayout::endRow()
{
    if (lastLayoutElement.itemAlignment == ITEMS_CENTERED || lastLayoutElement.itemAlignment == ITEMS_LEFT) {
        ImGui::Spring(0.5f);
    }
    ImGui::EndHorizontal();
    ImGui::PopStyleVar();

    if (lastLayoutElement.showBoundaries) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_Border));
    }
}
