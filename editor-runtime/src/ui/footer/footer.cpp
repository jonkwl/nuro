#include "footer.h"

Footer::Footer() : style(),
footerPosition(ImVec2(0.0f, 0.0f)),
footerSize(ImVec2(0.0f, 0.0f))
{
}

void Footer::render(const ImGuiViewport& viewport)
{
    footerPosition = ImVec2(viewport.Pos.x, viewport.Pos.y + viewport.Size.y - style.height);
    footerSize = ImVec2(viewport.Size.x, style.height);

    ImGui::SetNextWindowPos(footerPosition);
    ImGui::SetNextWindowSize(footerSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("TitleBar", nullptr, flags)) {

        ImDrawList& drawList = *ImGui::GetForegroundDrawList();
        renderContent(drawList);

    }
    ImGui::End();
    ImGui::PopStyleVar();;
}

FooterStyle& Footer::getStyle()
{
	return style;
}

void Footer::renderContent(ImDrawList& drawList)
{
    //
    // INVALID FONTS; RETURN
    // 

    if (!style.primaryFont || !style.secondaryFont) return;

    //
    // DRAW BACKGROUND
    //

    drawList.AddRectFilled(footerPosition, footerPosition + footerSize, style.backgroundColor);

    //
    // DRAW SAMPLE VERSION TEXT
    //

    const char* text = "Pre-Release Alpha 0.0.1";
    float fontSize = style.secondaryFont->FontSize;
    ImVec2 textSize = style.secondaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text) + style.padding;
    ImVec2 textPos = ImVec2(footerPosition.x + footerSize.x - textSize.x - style.padding.x, footerPosition.y + (footerSize.y - textSize.y) * 0.5f);
    drawList.AddText(style.secondaryFont, fontSize, textPos, style.secondaryTextColor, text);
}
