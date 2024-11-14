#include "scene_window.h"

void SceneWindow::prepare()
{
	ImGui::Begin("  Scene  ", nullptr, EngineUI::windowFlags.standard);
    
    UIComponents::headline("Scene", ICON_FA_MAP, HEADLINE_CENTER);
    
    UILayout::beginFlex("toggles", ROW, FULL_WIDTH, 60.0f, JUSTIFY_CENTER, ALIGN_CENTER, 1.0f);
    
        UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, Runtime::wireframe, "Wireframe");
        UIComponents::toggleButton(ICON_FA_CUBE, Runtime::solidMode, "Solid Mode");
        UIComponents::toggleButton(ICON_FA_ECLIPSE, Runtime::shadows, "Shadows");
        UIComponents::toggleButton(ICON_FA_SPARKLES, Runtime::postProcessingEffects, "Post Processing");
    
    UILayout::endFlex();

    UIComponents::input("Range: ", Runtime::range);
    UIComponents::input("Falloff: ", Runtime::falloff);

	ImGui::End();
}