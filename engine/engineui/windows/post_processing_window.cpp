#include "post_processing_window.h"

void PostProcessingWindow::prepare()
{
	ImGui::Begin("  Post Processing  ", nullptr, EngineUI::windowFlags.standard);

	UIComponents::headline("Post Processing", ICON_FA_SPARKLES, HEADLINE_LEFT);

	if (UIComponents::extendableSettings("Color Grading", PostProcessing::configuration.colorGrading, ICON_FA_PROJECTOR)) {
		
		UIComponents::headline("Contrast, Exposure & Gamma");
		UIComponents::input("Contrast:", PostProcessing::configuration.contrast, 0.0001f);
		UIComponents::input("Exposure:", PostProcessing::configuration.exposure, 0.001f);
		UIComponents::input("Gamma:", PostProcessing::configuration.gamma, 0.001f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Bloom", PostProcessing::configuration.bloom, ICON_FA_PROJECTOR)) {
		
		UIComponents::headline("Bloom Settings");
		UIComponents::input("Intensity:", PostProcessing::configuration.bloomIntensity, 0.001f);
		UIComponents::input("Threshold:", PostProcessing::configuration.bloomThreshold, 0.001f);
		UIComponents::input("Soft Threshold:", PostProcessing::configuration.bloomSoftThreshold, 0.001f);
		UIComponents::input("Filter Radius:", PostProcessing::configuration.bloomFilterRadius, 0.001f);
		UIComponents::colorPicker("Bloom Color", PostProcessing::configuration.bloomColor);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Lens Dirt Settings");
		UIComponents::input("Enable Lens Dirt", PostProcessing::configuration.lensDirt);
		UIComponents::input("Intensity:", PostProcessing::configuration.lensDirtIntensity, 0.01f);

		UIComponents::space(0.0f, 8.0f);

	}

	if (UIComponents::extendableSettings("Vignette", PostProcessing::configuration.vignette, ICON_FA_BAG_SHOPPING)) {

		UIComponents::headline("Vignette Settings");
		UIComponents::input("Intensity:", PostProcessing::configuration.vignetteIntensity, 0.01f);
		UIComponents::input("Radius:", PostProcessing::configuration.vignetteRadius, 0.01f);
		UIComponents::input("Softness:", PostProcessing::configuration.vignetteSoftness, 0.01f);
		UIComponents::input("Roundness:", PostProcessing::configuration.vignetteRoundness, 0.01f);
		UIComponents::colorPicker("Vignette Color", PostProcessing::configuration.vignetteColor);

		UIComponents::space(0.0f, 8.0f);

	}

	if (UIComponents::extendableSettings("Chromatic Aberration", PostProcessing::configuration.chromaticAberration, ICON_FA_BAG_SHOPPING)) {

		UIComponents::headline("Chromatic Aberration Settings");
		UIComponents::input("Intensity:", PostProcessing::configuration.chromaticAberrationIntensity, 0.01f);
		UIComponents::input("Range:", PostProcessing::configuration.chromaticAberrationRange, 0.01f);
		UIComponents::input("Red Offset:", PostProcessing::configuration.chromaticAberrationRedOffset, 0.01f);
		UIComponents::input("Blue Offset:", PostProcessing::configuration.chromaticAberrationBlueOffset, 0.01f);

		UIComponents::space(0.0f, 8.0f);

	}

	if (UIComponents::extendableSettings("Ambient Occlusion", PostProcessing::configuration.ambientOcclusion, ICON_FA_PROJECTOR)) {
		UIComponents::space(0.0f, 8.0f);
	}

	ImGui::End();
}
