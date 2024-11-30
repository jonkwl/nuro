#include "scene_window.h"

#include <imgui.h>

#include "../src/editor/engine_ui.h"
#include "../src/editor/ui_layout.h"
#include "../src/editor/ui_components.h"
#include "../src/editor/IconsFontAwesome6.h"
#include "../src/runtime/runtime.h"
#include "../src/rendering/postprocessing/post_processing.h"

void SceneWindow::prepare()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("  Scene  ", nullptr, EditorUI::windowFlags.standard);
	{
		UIComponents::space(0.0f, 15.0f);
		UIComponents::headline("Scene", ICON_FA_MAP, HeadlineJustification::CENTER);

		UILayout::beginFlex("toggles", FlexType::ROW, UILayout::FULL_WIDTH, 40.0f, Justification::CENTER, Alignment::CENTER, 1.0f);
		{

			UIComponents::toggleButton(ICON_FA_VECTOR_SQUARE, Runtime::wireframe, "Wireframe");
			UIComponents::toggleButton(ICON_FA_CUBE, Runtime::solidMode, "Solid Mode");
			UIComponents::toggleButton(ICON_FA_ECLIPSE, Runtime::shadows, "Shadows");
			UIComponents::toggleButton(ICON_FA_SPARKLES, Runtime::postProcessingEffects, "Post Processing");
		}
		UILayout::endFlex();

		glm::vec2 contentRegionAvail = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

		// Check if scene view has been resized
		if (contentRegionAvail != lastContentRegionAvail && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			Runtime::resizeViewport(contentRegionAvail.x, contentRegionAvail.y);

			lastContentRegionAvail = contentRegionAvail;
		}

		ImGui::BeginChild("SceneView", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImGui::Image(
				Runtime::postProcessingPipeline.getOutput(),
				ImGui::GetContentRegionAvail(),
				ImVec2(0, 1),
				ImVec2(1, 0));


			Runtime::sceneViewRightclick = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);

		}
		ImGui::EndChild();

		ImGui::End();
	}

	ImGui::PopStyleVar();
}