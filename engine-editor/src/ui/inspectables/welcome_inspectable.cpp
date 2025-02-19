#include "welcome_inspectable.h"

#include "../src/ui/editor_ui.h"
#include "../src/runtime/runtime.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/windows/insight_panel_window.h"
#include "../src/ui/components/inspectable_components.h"

WelcomeInspectable::WelcomeInspectable() : consoleSnapshot(),
playedGame(false)
{
    // Take snapshot of current console
    consoleSnapshot = ConsoleWindow::snapshot();
}

void WelcomeInspectable::renderStaticContent(ImDrawList& drawList)
{
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    IMComponents::label("Welcome!", EditorUI::getFonts().h1_bold, IM_COL32(200, 180, 255, 255));
    
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    IMComponents::label("Looks like you successfully built Nuro.", EditorUI::getFonts().h3_bold);
    
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Separator();

    if (!playedGame) {
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        IMComponents::label("Try out the demo scene!", EditorUI::getFonts().h2_bold, IM_COL32(180, 180, 255, 255));

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        if (IMComponents::buttonBig("Play Game", IM_COL32(50, 50, 80, 255))) Runtime::startGame();

        if (Runtime::getGameState() == GameState::GAME_RUNNING) playedGame = true;
    }
    else {
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        IMComponents::label("Demo game controls:", EditorUI::getFonts().h2_bold, IM_COL32(180, 180, 255, 255));

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        IMComponents::label("Use WASD to move around.", EditorUI::getFonts().h3_bold);

        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        IMComponents::label("Press the left mouse button to jump.", EditorUI::getFonts().h3_bold);

        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        IMComponents::label("Use the mouse wheel to zoom in and out.", EditorUI::getFonts().h3_bold);
    }
    
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    IMComponents::label("What's next?", EditorUI::getFonts().h3_bold);
    
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    IMComponents::label("Please take a look at the engine startup issue logs below to spot any issues.", EditorUI::getFonts().h4);
    
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    IMComponents::label("Note:", EditorUI::getFonts().h4_bold);
    IMComponents::label("Some engine assets are currently not included in the open-source repository.", EditorUI::getFonts().h4);

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    IMComponents::label("Issues:", EditorUI::getFonts().h3_bold);
}

void WelcomeInspectable::renderDynamicContent(ImDrawList& drawList)
{
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