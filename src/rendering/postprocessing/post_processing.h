#pragma once

#include "../src/rendering/texture/texture.h"

namespace PostProcessing
{

	struct Color {

		float exposure = 1.0f;
		float contrast = 1.004f;
		float gamma = 2.2f;

	};

	struct MotionBlur {

		bool enabled = true;

		bool cameraEnabled = true;
		float cameraIntensity = 0.8f;
		int cameraSamples = 32;

		bool objectEnabled = true;
		int objectSamples = 16;
		bool objectSilhouetteExtension = false;

	};

	struct Bloom {

		bool enabled = true;
		float intensity = 0.38f;
		float color[3] = { 1.0f, 1.0f, 1.0f };
		float threshold = 0.2f;
		float softThreshold = 0.0f;
		float filterRadius = 0.0f;
		unsigned int mipDepth = 16;

		bool lensDirtEnabled = false;
		Texture lensDirtTexture = Texture::empty();
		float lensDirtIntensity = 0.0f;

	};

	struct ChromaticAberration {

		bool enabled = true;
		float intensity = 0.055f;
		int iterations = 6;

	};

	struct Vignette {

		bool enabled = true;
		float intensity = 1.0f;
		float color[3] = { 0.0f, 0.0f, 0.0f };
		float radius = 0.68f;
		float softness = 0.35f;
		float roundness = 1.8f;

	};

	struct AmbientOcclusion {

		bool enabled = false;
		float radius = 0.2f;
		int samples = 64;
		float power = 20.0f;
		float bias = 0.03f;

	};

	inline Color color;
	inline MotionBlur motionBlur;
	inline Bloom bloom;
	inline ChromaticAberration chromaticAberration;
	inline Vignette vignette;
	inline AmbientOcclusion ambientOcclusion;

}