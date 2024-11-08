#include "scene_view.h"

void SceneView::prepare()
{
	ImGui::Begin("Scene View", nullptr, EngineUI::windowFlags.fixed);

	EngineUI::headline("Scene View");

	UIComponents::toggleButton(wireframe, ICON_FA_GHOST, 0, "Wireframe");
	ImGui::SameLine();
	UIComponents::toggleButton(solidMode, ICON_FA_SQUARE, 1, "Solid Mode");
	ImGui::SameLine();
	UIComponents::toggleButton(shadows, ICON_FA_CHESS_ROOK, 2, "Shadows");

	ImGui::End();
}
