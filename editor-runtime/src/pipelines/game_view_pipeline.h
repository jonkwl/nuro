#pragma once

#include <vector>

#include "../core/viewport/viewport.h"
#include "../core/old_entity/old_entity.h"
#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"
#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/core/forward_pass.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/gizmos/gizmos.h"
#include "../core/ecs/components.h"

class Skybox;

class GameViewPipeline
{
public:
	GameViewPipeline();

	void setup(); // Setup scene view pipeline

	void render(std::vector<OldEntity*>& targets); // Renders scene view

	unsigned int getOutput(); // Returns color output from latest render

	const Viewport& getViewport();
	void resizeViewport(float width, float height);

	void updateMsaaSamples(unsigned int msaaSamples);

	PostProcessing::Profile& getProfile();

	void setSkybox(Skybox* skybox);
	Skybox* getSkybox();

private:
	void createPasses(); // Create all passes
	void destroyPasses(); // Destroy all passes

	// Members
	Viewport viewport;
	PostProcessing::Profile profile;
	Skybox* skybox;

	// Render settings
	unsigned int msaaSamples; // MSAA Samples

	// Linked passes
	PrePass prePass;
	ForwardPass forwardPass;
	SSAOPass ssaoPass;
	VelocityBuffer velocityBuffer;
	PostProcessingPipeline postProcessingPipeline;
};

