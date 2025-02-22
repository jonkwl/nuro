#include "search_popup.h"

#include <string>
#include <cstdint>

#include "../src/ui/editor_ui.h"
#include "../src/ui/components/im_components.h"
#include "../src/core/rendering/icons/icon_pool.h"
#include "../src/editor_systems/component_registry.h"

namespace SearchPopup {

    // Set if search popup is shown
    bool gShow = false;

    // Position of current search popup
    ImVec2 gSearchPosition = ImVec2(0.0f, 0.0f);

    // Name of current search type
    std::string gSearchName;

    // Set if search popup was opened this frame
    bool gNewlyOpened;

    // Search buffer
    char gSearchBuffer[128];

    // Renders a search item element
    void _searchItem(std::string label, std::string icon) {

        //
        // EVALUATE
        //

        ImFont* font = EditorUI::getFonts().p_bold;
        float fontSize = font->FontSize;
        ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, label.c_str());

        float yPadding = 10.0f;
        ImVec2 position = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, textSize.y + yPadding * 2.0f);

        ImVec2 p0 = position;
        ImVec2 p1 = position + size;

        bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
        bool clicked = hovered && ImGui::IsMouseClicked(0);

        ImDrawList& drawList = *ImGui::GetWindowDrawList();

        //
        // DRAW BACKGROUND
        //

        ImU32 color = hovered ? IM_COL32(50, 50, 180, 70) : IM_COL32(50, 50, 68, 50);
        drawList.AddRectFilled(p0, p1, color, 10.0f);

        //
        // DRAW ICON
        //

        ImVec2 iconSize = ImVec2(fontSize * 1.65f, fontSize * 1.65f);
        ImVec2 iconPosition = position += ImVec2(15.0f, (size.y - iconSize.y) * 0.5f);
        drawList.AddImage(IconPool::get(icon), iconPosition, iconPosition + iconSize, ImVec2(0, 1), ImVec2(1, 0));

        //
        // DRAW TEXT
        //

        ImVec2 textPosition = position += ImVec2(iconSize.x + 8.0f, yPadding * 0.5f);
        drawList.AddText(font, fontSize, textPosition, IM_COL32_WHITE, label.c_str());

        //
        // ADVANCE CURSOR
        //

        ImGui::Dummy(ImVec2(0.0f, size.y));

        // 
        // HANDLE CLICKED EVENT
        //

        // ...
    }

    void render()
    {
        //
        // CHECK FOR OPENED POPUP
        //

        if (gShow) {
            ImGui::OpenPopup("Search");
            ImGui::SetNextWindowPos(gSearchPosition, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(440.0f, 400.0f));
        }

        //
        // DRAW POPUP MODAL
        //

        if (ImGui::BeginPopupModal("Search", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
        {
            //
            // TITLE
            //

            std::string title = "Search " + gSearchName;
            IMComponents::label(title.c_str(), EditorUI::getFonts().h3_bold);
            ImGui::Dummy(ImVec2(0.0f, 1.0f));

            //
            // HINT
            //

            IMComponents::label("Press esc to cancel, click to select.", EditorUI::getFonts().h4_bold);
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            //
            // SEARCH BAR
            //

            ImGui::PushFont(EditorUI::getFonts().h3);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f));
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            if (gNewlyOpened) ImGui::SetKeyboardFocusHere();
            if (ImGui::InputTextWithHint("##Search", "Search...", gSearchBuffer, IM_ARRAYSIZE(gSearchBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {

            }

            ImGui::PopFont();
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            //
            // SEARCH RESULTS
            //

            uint32_t nSearchResults = 10;
            IMComponents::beginClippedChild(ImGui::GetContentRegionAvail());
            {
                // TMP JUST RENDER COMPONENTS FOR NOW
                for (const auto& [name, component] : ComponentRegistry::get()) {
                    _searchItem(component.name, component.icon);
                }
            }
            IMComponents::endClippedChild();

            //
            // CHECK FOR CLOSING
            //

            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) close();

            //
            // END POPUP
            //

            gNewlyOpened = false;
            ImGui::EndPopup();
        }
    }

    void close()
    {
        gShow = false;
        ImGui::CloseCurrentPopup();
    }

    void searchComponents(ImVec2 position)
	{
        gShow = true;
        gSearchPosition = position;
        gSearchName = "Components";
        gNewlyOpened = true;
	}

}