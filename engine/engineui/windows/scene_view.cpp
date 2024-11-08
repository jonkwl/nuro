#include "scene_view.h"

void SceneView::prepare()
{
	ImGui::Begin("  Scene View  ", nullptr, EngineUI::windowFlags.standard);

        UIComponents::headline("Scene View", ICON_FA_MAP, HEADLINE_CENTERED);

        UILayout::beginRow("toggles", FULL_WIDTH, 60.0f, ITEMS_CENTERED, 0.5f, 1.0f, false);

            UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, Runtime::wireframe, "Wireframe");
            UIComponents::toggleButton(ICON_FA_CUBE, Runtime::solidMode, "Solid Mode");
            UIComponents::toggleButton(ICON_FA_ECLIPSE, Runtime::shadows, "Shadows");
            UIComponents::toggleButton(ICON_FA_SPARKLES, Runtime::postProcessingEffects, "Post Processing");

        UILayout::endRow();

        UIComponents::input("Range: ", Runtime::range);
        UIComponents::input("Falloff: ", Runtime::falloff);

	ImGui::End();
}
