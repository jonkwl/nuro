#include "footer.h"

#include <algorithm>

#include <context/application_context.h>
#include <utils/ioutils.h>

#include <utils/console.h>

#include "../ui/components/im_components.h"

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

    // Default text and state
    bool loading = false;
    std::string informationText = "No pending assets.";

    // Fetch worker state
    ResourceManager& resource = ApplicationContext::resourceManager();
    auto worker = resource.readWorkerState();

    // Fetch target resource if worker is active
    if (worker.targetId) {
        auto resourceOpt = resource.getResource(worker.targetId);
        if (resourceOpt) {
            Resource* resource = *resourceOpt;
            informationText = "Loading '" + resource->resourceName() + "'... (" + std::to_string(worker.tasksPending) + " remaining)";
            loading = true;
        }
    }

    // Draw text and loading buffer
    {
        // Draw loading buffer
        float bufferSize = style.height * 0.4f;
        if (loading) {
            ImVec2 bufferPos = ImVec2(footerPosition.x + footerSize.x - bufferSize - style.padding.x, footerPosition.y + (footerSize.y - bufferSize) * 0.5f);
            IMComponents::loadingBuffer(drawList, bufferPos, bufferSize * 0.5f, 2, style.primaryTextColor);
        }

        // Draw text
        float fontSize = style.primaryFont->FontSize;
        ImVec2 textPadding = loading ? ImVec2(bufferSize + 16.0f + style.padding.x, style.padding.y) : style.padding;
        ImVec2 textSize = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, informationText.c_str()) + textPadding;
        ImVec2 textPos = ImVec2(footerPosition.x + footerSize.x - textSize.x, footerPosition.y + (footerSize.y - textSize.y) * 0.5f);
        drawList.AddText(style.primaryFont, fontSize, textPos, style.primaryTextColor, informationText.c_str());
    }

    //
    // DRAW SAMPLE VERSION TEXT
    //

    {
        const char* versionText = "Pre-Release Alpha 0.0.1";
        float fontSize = style.primaryFont->FontSize;
        ImVec2 textSize = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, versionText) + style.padding;
        ImVec2 textPos = ImVec2(footerPosition.x + style.padding.x, footerPosition.y + (footerSize.y - textSize.y) * 0.5f);
        drawList.AddText(style.primaryFont, fontSize, textPos, style.secondaryTextColor, versionText);
    }
}
