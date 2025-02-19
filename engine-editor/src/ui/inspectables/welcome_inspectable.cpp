#include "welcome_inspectable.h"

#include "../src/ui/editor_ui.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/windows/insight_panel_window.h"
#include "../src/ui/components/inspectable_components.h"

WelcomeInspectable::WelcomeInspectable()
{
    // Take snapshot of current console
    consoleSnapshot = ConsoleWindow::snapshot();
}

void WelcomeInspectable::renderStaticContent(ImDrawList& drawList)
{
    IMComponents::label("Welcome!", EditorUI::getFonts().h1_bold, IM_COL32(200, 180, 255, 255));
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    IMComponents::label("Looks like you successfully built Nuro.", EditorUI::getFonts().h3);
    
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    IMComponents::label("What's next?", EditorUI::getFonts().h3_bold);
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    IMComponents::label("Take a look at the engine startup issue logs below to spot any issues.", EditorUI::getFonts().h4);
}

void WelcomeInspectable::renderDynamicContent(ImDrawList& drawList)
{
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    IMComponents::label("Issues:", EditorUI::getFonts().h3_bold);

    // Amount of issues logged
    uint32_t nIssues = 0;

    // Display all logs except for normal message logs
    for (ConsoleLog& log : consoleSnapshot) {
        if (log.type != ConsoleLogType::MESSAGE) {
            IMComponents::label(ICON_FA_TRIANGLE_EXCLAMATION " " + log.content, EditorUI::getFonts().h4, IM_COL32(255, 180, 15, 255));
            nIssues++;
        }
    }

    // No issues logged
    if (!nIssues) {
        IMComponents::label(ICON_FA_CHECK " No issues found", EditorUI::getFonts().h4, IM_COL32(160, 255, 160, 255));
    }
}