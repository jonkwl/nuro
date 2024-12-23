#pragma once

#include <vector>

#include "../core/viewport/viewport.h"
#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"
#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/core/forward_pass.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/gizmos/gizmos.h"

class Skybox;

class GameViewPipeline
{
public:
	GameViewPipeline();

	void create(); // Create game view pipeline
	void destroy(); // Destroy game view pipeline

	void tryRender(); // Renders game view if conditions are met

	uint32_t getOutput(); // Returns color output from latest render

	const Viewport& getViewport();
	void resizeViewport(float width, float height);

	void updateMsaaSamples(uint32_t msaaSamples);

	PostProcessing::Profile& getProfile();

	void linkSkybox(Skybox* skybox);
	Skybox* getLinkedSkybox();
	bool drawSkybox;

	void linkGizmos(IMGizmo* gizmos);
	bool drawGizmos;

	bool getCameraAvailable();

	uint32_t getSSAOOutput() const; // Returns the last ssao output
	uint32_t getVelocityOutput() const; // Returns the velocity buffers last output

private:
	void render(); // Renders game view

	void createPasses(); // Create all passes
	void destroyPasses(); // Destroy all passes

	// Members
	Viewport viewport;
	PostProcessing::Profile profile;
	Skybox* skybox; // Optional skybox
	IMGizmo* gizmos; // Optional gizmos

	// Render settings
	uint32_t msaaSamples; // MSAA Samples

	// Linked passes
	PrePass prePass;
	ForwardPass forwardPass;
	SSAOPass ssaoPass;
	VelocityBuffer velocityBuffer;
	PostProcessingPipeline postProcessingPipeline;

	// Camera available cache
	bool cameraAvailable;

	// Cached outputs
	uint32_t ssaoOutput;
	uint32_t velocityOutput;
};

