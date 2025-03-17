#include "welcome_inspectable.h"

#include <functional>

#include "../ui/editor_ui.h"
#include "../runtime/runtime.h"
#include "../ui/windows/registry_window.h"
#include "../ui/components/im_components.h"
#include "../ui/windows/insight_panel_window.h"
#include "../ui/components/inspectable_components.h"

WelcomeInspectable::WelcomeInspectable() : consoleSnapshot(),
playedGame(false),
loggedIssues(),
logSelect(
    std::bind(&WelcomeInspectable::logLineAt, this, std::placeholders::_1), 
    std::bind(&WelcomeInspectable::nLogs, this))
{
    // Take snapshot of current console
    consoleSnapshot = ConsoleWindow::snapshot();

    // Fetch logs from console snapshot
    for (ConsoleLog& log : consoleSnapshot) {
        if (log.type != LogType::DEFAULT) {
            loggedIssues.push_back(log.content);
        }
    }
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
    // Issues were logged
    if (loggedIssues.size()) {
        ImGui::BeginChild("logs", {}, 0, ImGuiWindowFlags_NoMove);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 180, 15, 255));
            for (const auto& line : loggedIssues) {
                std::string logText = std::string(line.data()) + "  " + ICON_FA_TRIANGLE_EXCLAMATION;
                ImGui::TextUnformatted(logText.c_str());
            }
            ImGui::PopStyleColor();

            logSelect.update();

            if (ImGui::BeginPopupContextWindow()) {
                ImGui::BeginDisabled(!logSelect.hasSelection());
                if (ImGui::MenuItem("Copy", "Ctrl+C")) logSelect.copy();
                ImGui::EndDisabled();
                if (ImGui::MenuItem("Select all", "Ctrl+A")) logSelect.selectAll();
                ImGui::EndPopup();
            }
        }
        ImGui::EndChild();
    }
    // No issues were logged
    else {
        IMComponents::label(ICON_FA_CHECK " No issues found", EditorUI::getFonts().h4, IM_COL32(160, 255, 160, 255));
    }
}

std::string_view WelcomeInspectable::logLineAt(size_t i)
{
    return loggedIssues[i];
}

size_t WelcomeInspectable::nLogs()
{
    return loggedIssues.size();
}
