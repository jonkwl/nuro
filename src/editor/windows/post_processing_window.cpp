#include "post_processing_window.h"

#include <imgui.h>

#include "../src/editor/ui_layout.h"
#include "../src/editor/ui_components.h"
#include "../src/editor/IconsFontAwesome6.h"
#include "../src/runtime/runtime.h"
#include "../src/editor/editor_ui.h"

bool post_processing_window_tmp = false;

void PostProcessingWindow::prepare()
{
	ImGui::Begin("  Post Processing  ", nullptr, EditorUI::getWindowFlags().standard);

	UIComponents::headline("Post Processing", ICON_FA_SPARKLES, HeadlineJustification::LEFT);

	if (UIComponents::extendableSettings("Color Grading", post_processing_window_tmp, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Contrast, Exposure & Gamma");
		UIComponents::input("Contrast:", PostProcessing::color.contrast, 0.0001f);
		UIComponents::input("Exposure:", PostProcessing::color.exposure, 0.001f);
		UIComponents::input("Gamma:", PostProcessing::color.gamma, 0.001f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Motion Blur", PostProcessing::motionBlur.enabled, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Camera Motion Blur Settings");
		UIComponents::input("Enable Camera Motion Blur", PostProcessing::motionBlur.cameraEnabled);
		UIComponents::input("Intensity:", PostProcessing::motionBlur.cameraIntensity, 0.001f);
		UIComponents::input("Samples:", PostProcessing::motionBlur.cameraSamples, 0.1f);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Object Motion Blur Settings");
		UIComponents::input("Enable Object Motion Blur", PostProcessing::motionBlur.objectEnabled);
		UIComponents::input("Samples:", PostProcessing::motionBlur.objectSamples, 0.1f);

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

	if (UIComponents::extendableSettings("Bloom", PostProcessing::bloom.enabled, ICON_FA_PROJECTOR))
	{

		UIComponents::headline("Bloom Settings");
		UIComponents::input("Intensity:", PostProcessing::bloom.intensity, 0.001f);
		UIComponents::input("Threshold:", PostProcessing::bloom.threshold, 0.001f);
		UIComponents::input("Soft Threshold:", PostProcessing::bloom.softThreshold, 0.001f);
		UIComponents::input("Filter Radius:", PostProcessing::bloom.filterRadius, 0.001f);
		UIComponents::colorPicker("Bloom Color", PostProcessing::bloom.color);

		UIComponents::space(0.0f, 5.0f);
		UIComponents::headline("Lens Dirt Settings");
		UIComponents::input("Enable Lens Dirt", PostProcessing::bloom.lensDirtEnabled);
		UIComponents::input("Intensity:", PostProcessing::bloom.lensDirtIntensity, 0.01f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Vignette", PostProcessing::vignette.enabled, ICON_FA_BAG_SHOPPING))
	{

		UIComponents::headline("Vignette Settings");
		UIComponents::input("Intensity:", PostProcessing::vignette.intensity, 0.01f);
		UIComponents::input("Radius:", PostProcessing::vignette.radius, 0.01f);
		UIComponents::input("Softness:", PostProcessing::vignette.softness, 0.01f);
		UIComponents::input("Roundness:", PostProcessing::vignette.roundness, 0.01f);
		UIComponents::colorPicker("Vignette Color", PostProcessing::vignette.color);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Chromatic Aberration", PostProcessing::chromaticAberration.enabled, ICON_FA_BAG_SHOPPING))
	{

		UIComponents::headline("Chromatic Aberration Settings");
		UIComponents::input("Intensity:", PostProcessing::chromaticAberration.intensity, 0.01f);
		UIComponents::input("Iterations:", PostProcessing::chromaticAberration.iterations, 0.1f);

		UIComponents::space(0.0f, 8.0f);
	}

	if (UIComponents::extendableSettings("Ambient Occlusion", PostProcessing::ambientOcclusion.enabled, ICON_FA_PROJECTOR))
	{
		UIComponents::headline("SSAO Settings");
		UIComponents::input("Radius:", PostProcessing::ambientOcclusion.radius, 0.001f);
		UIComponents::input("Samples:", PostProcessing::ambientOcclusion.samples, 0.1f);
		UIComponents::input("Power:", PostProcessing::ambientOcclusion.power, 0.001f);
		UIComponents::input("Bias:", PostProcessing::ambientOcclusion.bias, 0.0001f);

		UIComponents::space(0.0f, 8.0f);
	}

	ImGui::End();
}
