#include "workspace_bar.h"

WorkspaceBar::WorkspaceBar() : style(),
position(ImVec2(0.0f, 0.0f)),
size(ImVec2(0.0f, 0.0f))
{
}

void WorkspaceBar::render(const ImGuiViewport& viewport, float yOffset)
{
    position = viewport.Pos + ImVec2(0.0f, yOffset);
    size = ImVec2(viewport.Size.x, style.height);

    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("WorkspaceBar", nullptr, flags)) {

        ImDrawList& drawList = *ImGui::GetForegroundDrawList();
        renderContent(drawList);

    }
    ImGui::End();
    ImGui::PopStyleVar();;
}

WorkspaceBarStyle& WorkspaceBar::getStyle()
{
	return style;
}

void WorkspaceBar::renderContent(ImDrawList& drawList)
{
    //
    // INVALID FONTS; RETURN
    // 

    if (!style.primaryFont || !style.secondaryFont) return;

    //
    // DRAW BACKGROUND
    //

    drawList.AddRectFilled(position, position + size, style.backgroundColor);

    //
    // DRAW SAMPLE VERSION TEXT
    //

    const char* text = "Example workspace bar";
    float fontSize = style.secondaryFont->FontSize;
    ImVec2 textSize = style.secondaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text) + style.padding;
    ImVec2 textPos = ImVec2(position.x + style.padding.x, position.y + (size.y - textSize.y) * 0.5f);
    drawList.AddText(style.secondaryFont, fontSize, textPos, style.secondaryTextColor, text);

    //
    // DRAW TOP BORDER
    //

    ImVec2 borderStart = position;
    ImVec2 borderEnd = borderStart + ImVec2(size.x, 0.0f);
    drawList.AddLine(borderStart, borderEnd, style.borderColor, style.borderThickness);
}
