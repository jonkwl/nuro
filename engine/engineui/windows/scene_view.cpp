#include "scene_view.h"

void SceneView::prepare()
{
	ImGui::Begin("Scene View", nullptr, EngineUI::windowFlags.standard);

	UIComponents::headline("Scene View", ICON_FA_MAP);

	UIComponents::toggleButton(Runtime::wireframe, ICON_FA_VECTOR_SQUARE, 0, "Wireframe");
	ImGui::SameLine();
	UIComponents::toggleButton(Runtime::solidMode, ICON_FA_CUBE, 1, "Solid Mode");
	ImGui::SameLine();
	UIComponents::toggleButton(Runtime::shadows, ICON_FA_ECLIPSE, 2, "Shadows");
	ImGui::SameLine();
	UIComponents::toggleButton(Runtime::postProcessingEffects, ICON_FA_SPARKLES, 3, "Post Processing");

	ImGui::End();
}
