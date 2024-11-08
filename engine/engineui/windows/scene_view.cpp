#include "scene_view.h"

void SceneView::prepare()
{
	ImGui::Begin("Scene View", nullptr, EngineUI::windowFlags.fixed);

	ImGui::Text(ICON_FA_EARTH_AMERICAS);
	ImGui::SameLine();
	EngineUI::headline("Scene View");

	UIComponents::toggleButton(Runtime::wireframe, ICON_FA_GHOST, 0, "Wireframe");
	ImGui::SameLine();
	UIComponents::toggleButton(Runtime::solidMode, ICON_FA_SQUARE, 1, "Solid Mode");
	ImGui::SameLine();
	UIComponents::toggleButton(Runtime::shadows, ICON_FA_CHESS_ROOK, 2, "Shadows");

	ImGui::End();
}
