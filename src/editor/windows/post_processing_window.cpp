#include "post_processing_window.h"

#include <imgui.h>

#include "../src/editor/ui_layout.h"
#include "../src/editor/ui_components.h"
#include "../src/editor/IconsFontAwesome6.h"
#include "../src/runtime/runtime.h"
#include "../src/editor/editor_ui.h"

PostProcessingWindow::PostProcessingWindow() :
	configuration(PostProcessing::configuration)
{};

void PostProcessingWindow::prepare()
{
	ImGui::Begin("  Post Processing  ", nullptr, EditorUI::getWindowFlags().standard);

	UIComponents::headline("Post Processing", ICON_FA_SPARKLES, HeadlineJustification::LEFT);

	if (UIComponents::extendableSettings("Color Grading", configuration.colorGrading, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Contrast, Exposure & Gamma");
		UIComponents::input("Contrast:", configuration.contrast, 0.0001f);
		UIComponents::input("Exposure:", configuration.exposure, 0.001f);
		UIComponents::input("Gamma:", configuration.gamma, 0.001f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Motion Blur", configuration.motionBlur, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Camera Motion Blur Settings");
		UIComponents::input("Enable Camera Motion Blur", configuration.motionBlurCamera);
		UIComponents::input("Intensity:", configuration.motionBlurCameraIntensity, 0.001f);
		UIComponents::input("Samples:", configuration.motionBlurCameraSamples, 0.1f);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Object Motion Blur Settings");
		UIComponents::input("Enable Object Motion Blur", configuration.motionBlurObject);
		UIComponents::input("Samples:", configuration.motionBlurObjectSamples, 0.1f);

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

	if (UIComponents::extendableSettings("Bloom", configuration.bloom, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Bloom Settings");
		UIComponents::input("Intensity:", configuration.bloomIntensity, 0.001f);
		UIComponents::input("Threshold:", configuration.bloomThreshold, 0.001f);
		UIComponents::input("Soft Threshold:", configuration.bloomSoftThreshold, 0.001f);
		UIComponents::input("Filter Radius:", configuration.bloomFilterRadius, 0.001f);
		UIComponents::colorPicker("Bloom Color", configuration.bloomColor);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Lens Dirt Settings");
		UIComponents::input("Enable Lens Dirt", configuration.lensDirt);
		UIComponents::input("Intensity:", configuration.lensDirtIntensity, 0.01f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Vignette", configuration.vignette, ICON_FA_BAG_SHOPPING))
	{

		UIComponents::headline("Vignette Settings");
		UIComponents::input("Intensity:", configuration.vignetteIntensity, 0.01f);
		UIComponents::input("Radius:", configuration.vignetteRadius, 0.01f);
		UIComponents::input("Softness:", configuration.vignetteSoftness, 0.01f);
		UIComponents::input("Roundness:", configuration.vignetteRoundness, 0.01f);
		UIComponents::colorPicker("Vignette Color", configuration.vignetteColor);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Chromatic Aberration", configuration.chromaticAberration, ICON_FA_BAG_SHOPPING))
	{

		UIComponents::headline("Chromatic Aberration Settings");
		UIComponents::input("Intensity:", configuration.chromaticAberrationIntensity, 0.01f);
		UIComponents::input("Iterations:", configuration.chromaticAberrationIterations, 0.1f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Ambient Occlusion", configuration.ambientOcclusion, ICON_FA_PROJECTOR))
	{
		UIComponents::headline("SSAO Settings");
		UIComponents::input("Radius:", configuration.ambientOcclusionRadius, 0.001f);
		UIComponents::input("Samples:", configuration.ambientOcclusionSamples, 0.1f);
		UIComponents::input("Power:", configuration.ambientOcclusionPower, 0.001f);
		UIComponents::input("Bias:", configuration.ambientOcclusionBias, 0.0001f);

		UIComponents::space(0.0f, 8.0f);
	}

	ImGui::End();
}
