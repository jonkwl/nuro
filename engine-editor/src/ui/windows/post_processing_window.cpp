#include "post_processing_window.h"

#include "../src/ui/components/inspectable_components.h"

PostProcessingWindow::PostProcessingWindow(PostProcessing::Profile& targetProfile) : targetProfile(targetProfile)
{
}

void PostProcessingWindow::render()
{
	std::string title = UIUtils::windowTitle("Post Processing");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::standard);
	{
		IMComponents::headline("Post Processing", ICON_FA_SPARKLES);

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		IMComponents::label(ICON_FA_PENCIL "  Editing: Global Profile", EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		InspectableComponents::drawColor(targetProfile.color);
		InspectableComponents::drawMotionBlur(targetProfile.motionBlur);
		InspectableComponents::drawBloom(targetProfile.bloom);
		InspectableComponents::drawChromaticAberration(targetProfile.chromaticAberration);
		InspectableComponents::drawVignette(targetProfile.vignette);
		InspectableComponents::drawAmbientOcclusion(targetProfile.ambientOcclusion);
	}
	ImGui::End();
}

PostProcessing::Profile& PostProcessingWindow::getTargetProfile() {
	return targetProfile;
}

void PostProcessingWindow::updateTargetProfile(PostProcessing::Profile& profile) {
	targetProfile = profile;
}
