#include "post_processing_window.h"

PostProcessingWindow::PostProcessingWindow(PostProcessing::Profile& targetProfile) : targetProfile(targetProfile)
{
}

void PostProcessingWindow::render()
{
	ImGui::Begin("  Post Processing  ", nullptr, EditorUI::getWindowFlags().standard);

	UIComponents::headline("Post Processing", ICON_FA_SPARKLES, HeadlineJustification::LEFT);

	bool post_processing_window_tmp = false;

	if (UIComponents::extendableSettings("Color Grading", post_processing_window_tmp, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Contrast, Exposure & Gamma");
		UIComponents::input("Contrast:", targetProfile.color.contrast, 0.0001f);
		UIComponents::input("Exposure:", targetProfile.color.exposure, 0.001f);
		UIComponents::input("Gamma:", targetProfile.color.gamma, 0.001f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Motion Blur", targetProfile.motionBlur.enabled, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Camera Motion Blur Settings");
		UIComponents::input("Enable Camera Motion Blur", targetProfile.motionBlur.cameraEnabled);
		UIComponents::input("Intensity:", targetProfile.motionBlur.cameraIntensity, 0.001f);
		UIComponents::input("Samples:", targetProfile.motionBlur.cameraSamples, 0.1f);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Object Motion Blur Settings");
		UIComponents::input("Enable Object Motion Blur", targetProfile.motionBlur.objectEnabled);
		UIComponents::input("Samples:", targetProfile.motionBlur.objectSamples, 0.1f);

		/*UIComponents::space(0.0f, 2.0f);
		UILayout::beginFlex("SilhouetteExtension", ROW, FULL_WIDTH, 20.0f, JUSTIFY_START, ALIGN_CENTER);
		{
			UIComponents::tryIcon(ICON_FA_FLASK);
			UIComponents::labelBold("Experimental: ");
			UIComponents::input("Extend Silhouettes", configuration.motionBlurObjectSilhouetteExtension);
		}
		UILayout::endFlex();
		if (configuration.motionBlurObjectSilhouetteExtension) {
			UILayout::beginFlex("SilhouetteExtensionWarning", ROW, FULL_WIDTH, 20.0f, JUSTIFY_START, ALIGN_CENTER);
			{
				UIComponents::tryIcon(ICON_FA_TRIANGLE_EXCLAMATION);
				UIComponents::label("May introduce artifacts when blurred object is in background");
			}
			UILayout::endFlex();
		}*/

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Bloom", targetProfile.bloom.enabled, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Bloom Settings");
		UIComponents::input("Intensity:", targetProfile.bloom.intensity, 0.001f);
		UIComponents::input("Threshold:", targetProfile.bloom.threshold, 0.001f);
		UIComponents::input("Soft Threshold:", targetProfile.bloom.softThreshold, 0.001f);
		UIComponents::input("Filter Radius:", targetProfile.bloom.filterRadius, 0.001f);
		UIComponents::colorPicker("Bloom Color", targetProfile.bloom.color);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Lens Dirt Settings");
		UIComponents::input("Enable Lens Dirt", targetProfile.bloom.lensDirtEnabled);
		UIComponents::input("Intensity:", targetProfile.bloom.lensDirtIntensity, 0.01f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Vignette", targetProfile.vignette.enabled, ICON_FA_BAG_SHOPPING))
	{

		UIComponents::headline("Vignette Settings");
		UIComponents::input("Intensity:", targetProfile.vignette.intensity, 0.01f);
		UIComponents::input("Radius:", targetProfile.vignette.radius, 0.01f);
		UIComponents::input("Softness:", targetProfile.vignette.softness, 0.01f);
		UIComponents::input("Roundness:", targetProfile.vignette.roundness, 0.01f);
		UIComponents::colorPicker("Vignette Color", targetProfile.vignette.color);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Chromatic Aberration", targetProfile.chromaticAberration.enabled, ICON_FA_BAG_SHOPPING))
	{

		UIComponents::headline("Chromatic Aberration Settings");
		UIComponents::input("Intensity:", targetProfile.chromaticAberration.intensity, 0.01f);
		UIComponents::input("Iterations:", targetProfile.chromaticAberration.iterations, 0.1f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Ambient Occlusion", targetProfile.ambientOcclusion.enabled, ICON_FA_PROJECTOR))
	{
		UIComponents::headline("SSAO Settings");
		UIComponents::input("Radius:", targetProfile.ambientOcclusion.radius, 0.001f);
		UIComponents::input("Samples:", targetProfile.ambientOcclusion.samples, 0.1f);
		UIComponents::input("Power:", targetProfile.ambientOcclusion.power, 0.001f);
		UIComponents::input("Bias:", targetProfile.ambientOcclusion.bias, 0.0001f);

		UIComponents::space(0.0f, 8.0f);
	}

	ImGui::End();
}

PostProcessing::Profile& PostProcessingWindow::getTargetProfile() {
	return targetProfile;
}

void PostProcessingWindow::updateTargetProfile(PostProcessing::Profile& profile) {
	targetProfile = profile;
}
