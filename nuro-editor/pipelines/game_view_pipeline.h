#pragma once

#include <vector>

#include <viewport/viewport.h>
#include <rendering/gizmos/gizmos.h>
#include <rendering/passes/ssao_pass.h>
#include <rendering/passes/pre_pass.h>
#include <rendering/passes/forward_pass.h>
#include <rendering/passes/preprocessor_pass.h>
#include <rendering/velocitybuffer/velocity_buffer.h>
#include <rendering/postprocessing/post_processing.h>
#include <rendering/postprocessing/post_processing_pipeline.h>

class Skybox;

class GameViewPipeline
{
public:
	GameViewPipeline();

	// Create game view pipeline
	void create();

	// Destroy game view pipeline
	void destroy();

	// Renders game view
	void render();

	// Returns color output from latest render
	uint32_t getOutput();

	// Returns the viewport used
	const Viewport& getViewport() const;

	// Resizes the viewport used
	void resizeViewport(float width, float height);

	// Updates the msaa samples
	void updateMsaaSamples(uint32_t msaaSamples);

	// Returns the mutable post processing profile used
	PostProcessing::Profile& getProfile();

	// Sets given skybox to be linked
	void linkSkybox(Skybox* skybox);

	// Returns the currently linked skybox, may be nullptr if none is set
	Skybox* getLinkedSkybox();

	// The linked skybox will be drawn if this is set
	bool drawSkybox;

	// Sets given gizmo instance to be linked
	void linkGizmos(IMGizmo* gizmos);

	// Gizmos will be drawn if this is set
	bool drawGizmos;

	// Returns true if there was a camera render target available during the last render
	bool getCameraAvailable();

	// Returns the last ssao output
	uint32_t getSSAOOutput() const;

	// Returns the velocity buffers last output
	uint32_t getVelocityOutput() const;

private:
	// Create all passes
	void createPasses();

	// Destroy all passes
	void destroyPasses();

	//
	// General members
	//

	Viewport viewport;
	PostProcessing::Profile profile;
	Skybox* skybox; // Optional skybox
	IMGizmo* gizmos; // Optional gizmos

	//
	// Render settings
	//

	uint32_t msaaSamples;

	//
	// Passes
	//

	PreprocessorPass preprocessorPass;
	PrePass prePass;
	ForwardPass forwardPass;
	SSAOPass ssaoPass;
	VelocityBuffer velocityBuffer;
	PostProcessingPipeline postProcessingPipeline;

	//
	// States and outputs
	//

	bool cameraAvailable;

	uint32_t ssaoOutput;
	uint32_t velocityOutput;
};