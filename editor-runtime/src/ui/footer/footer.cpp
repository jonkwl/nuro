#include "footer.h"

#include "../core/context/application_context.h"
#include "../core/utils/iohandler.h"

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
    // DRAW LOADING INFORMATION TEXT
    //

    auto worker = ApplicationContext::getAssetLoader().readWorkerState();
    if (worker.currentTask) {
        std::string informationText = "Loading " + IOHandler::getFilename(worker.currentTask->sourcePath()) + "... (" + std::to_string(worker.tasksPending) + " remaining)";
        float fontSize = style.primaryFont->FontSize;
        ImVec2 textSize = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, informationText.c_str()) + style.padding;
        ImVec2 textPos = ImVec2(footerPosition.x + footerSize.x - textSize.x - style.padding.x, footerPosition.y + (footerSize.y - textSize.y) * 0.5f);
        drawList.AddText(style.primaryFont, fontSize, textPos, style.secondaryTextColor, informationText.c_str());
    }

    //
    // DRAW SAMPLE VERSION TEXT
    //

    /*const char* versionText = "Pre-Release Alpha 0.0.1";
    float fontSize = style.primaryFont->FontSize;
    ImVec2 textSize = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, versionText) + style.padding;
    ImVec2 textPos = ImVec2(footerPosition.x + footerSize.x - textSize.x - style.padding.x, footerPosition.y + (footerSize.y - textSize.y) * 0.5f);
    drawList.AddText(style.primaryFont, fontSize, textPos, style.secondaryTextColor, versionText);*/
}
