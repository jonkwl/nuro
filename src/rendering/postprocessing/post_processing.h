#pragma once

#include "../src/rendering/texture/texture.h"

namespace PostProcessing
{

	struct Configuration
	{
		bool colorGrading = false;
		float exposure = 1.0f;
		float contrast = 1.004f;
		float gamma = 2.2f;

		bool motionBlur = true;
		bool motionBlurCamera = true;
		float motionBlurCameraIntensity = 0.8f;
		int motionBlurCameraSamples = 32;
		bool motionBlurObject = true;
		int motionBlurObjectSamples = 16;
		bool motionBlurObjectSilhouetteExtension = false;

		bool bloom = true;
		float bloomIntensity = 0.38f;
		float bloomColor[3] = {1.0f, 1.0f, 1.0f};
		float bloomThreshold = 0.2f;
		float bloomSoftThreshold = 0.0f;
		float bloomFilterRadius = 0.0f;
		unsigned int bloomMipDepth = 16;
		bool lensDirt = false;
		Texture lensDirtTexture;
		float lensDirtIntensity = 0.0f;

		bool chromaticAberration = true;
		float chromaticAberrationIntensity = 0.055f;
		int chromaticAberrationIterations = 6;

		bool vignette = true;
		float vignetteIntensity = 1.0f;
		float vignetteColor[3] = {0.0f, 0.0f, 0.0f};
		float vignetteRadius = 0.68f;
		float vignetteSoftness = 0.35f;
		float vignetteRoundness = 1.8f;

		bool ambientOcclusion = true;
		float ambientOcclusionRadius = 0.2f;
		int ambientOcclusionSamples = 64;
		float ambientOcclusionPower = 20.0f;
		float ambientOcclusionBias = 0.03f;
	};

	extern Configuration configuration;

}