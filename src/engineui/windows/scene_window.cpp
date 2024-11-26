#include "scene_window.h"

#include <imgui.h>

#include "../src/engineui/engine_ui.h"
#include "../src/engineui/ui_layout.h"
#include "../src/engineui/ui_components.h"
#include "../src/engineui/IconsFontAwesome6.h"
#include "../src/runtime/runtime.h"
#include "../src/rendering/postprocessing/post_processing.h"

void SceneWindow::prepare()
{
	ImGui::Begin("  Scene  ", nullptr, EngineUI::windowFlags.standard);
    
    UIComponents::headline("Scene", ICON_FA_MAP, HeadlineJustification::CENTER);
    
    UILayout::beginFlex("toggles", FlexType::ROW, UILayout::FULL_WIDTH, 60.0f,Justification::CENTER, Alignment::CENTER, 1.0f);
    {
        
        UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, Runtime::wireframe, "Wireframe");
        UIComponents::toggleButton(ICON_FA_CUBE, Runtime::solidMode, "Solid Mode");
        UIComponents::toggleButton(ICON_FA_ECLIPSE, Runtime::shadows, "Shadows");
        UIComponents::toggleButton(ICON_FA_SPARKLES, Runtime::postProcessingEffects, "Post Processing");

    }
    UILayout::endFlex();

    ImGui::BeginChild("SceneView");
    {
        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetContentRegionAvail().y;

        ImGui::Image(
            PostProcessing::getOutput(),
            ImGui::GetContentRegionAvail(),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    }
    ImGui::EndChild();

	ImGui::End();
}