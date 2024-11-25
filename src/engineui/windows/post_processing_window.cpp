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

	if (UIComponents::extendableSettings("Motion Blur", PostProcessing::configuration.motionBlur, ICON_FA_PROJECTOR)) {

		UIComponents::headline("Camera Motion Blur Settings");
		UIComponents::input("Enable Camera Motion Blur", PostProcessing::configuration.motionBlurCamera);
		UIComponents::input("Intensity:", PostProcessing::configuration.motionBlurCameraIntensity, 0.001f);
		UIComponents::input("Samples:", PostProcessing::configuration.motionBlurCameraSamples, 0.1f);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Object Motion Blur Settings");
		UIComponents::input("Enable Object Motion Blur", PostProcessing::configuration.motionBlurObject);
		UIComponents::input("Samples:", PostProcessing::configuration.motionBlurObjectSamples, 0.1f);

		/*UIComponents::space(0.0f, 2.0f);
		UILayout::beginFlex("SilhouetteExtension", ROW, FULL_WIDTH, 20.0f, JUSTIFY_START, ALIGN_CENTER);
		{
			UIComponents::tryIcon(ICON_FA_FLASK);
			UIComponents::labelBold("Experimental: ");
			UIComponents::input("Extend Silhouettes", PostProcessing::configuration.motionBlurObjectSilhouetteExtension);
		}
		UILayout::endFlex();
		if (PostProcessing::configuration.motionBlurObjectSilhouetteExtension) {
			UILayout::beginFlex("SilhouetteExtensionWarning", ROW, FULL_WIDTH, 20.0f, JUSTIFY_START, ALIGN_CENTER);
			{
				UIComponents::tryIcon(ICON_FA_TRIANGLE_EXCLAMATION);
				UIComponents::label("May introduce artifacts when blurred object is in background");
			}
			UILayout::endFlex();
		}*/

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
		UIComponents::input("Iterations:", PostProcessing::configuration.chromaticAberrationIterations, 0.1f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Ambient Occlusion", PostProcessing::configuration.ambientOcclusion, ICON_FA_PROJECTOR)) {
		UIComponents::headline("SSAO Settings");
		UIComponents::input("Radius:", PostProcessing::configuration.ambientOcclusionRadius, 0.001f);
		UIComponents::input("Samples:", PostProcessing::configuration.ambientOcclusionSamples, 0.1f);
		UIComponents::input("Power:", PostProcessing::configuration.ambientOcclusionPower, 0.001f);
		UIComponents::input("Bias:", PostProcessing::configuration.ambientOcclusionBias, 0.0001f);

		UIComponents::space(0.0f, 8.0f);
	}

	ImGui::End();
}
