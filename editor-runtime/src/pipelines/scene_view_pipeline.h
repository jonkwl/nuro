#pragma once

#include <vector>

#include "../core/viewport/viewport.h"
#include "../core/old_entity/old_entity.h"
#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"
#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/sceneview/scene_view_forward_pass.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/rendering/gizmos/gizmos.h"
#include "../core/camera/camera.h"

class SceneViewPipeline
{
public:
	SceneViewPipeline();

	void setup(); // Setup scene view pipeline

	void render(std::vector<OldEntity*>& targets); // Renders scene view

	unsigned int getOutput(); // Returns color output from latest render

	const Viewport& getViewport();
	void resizeViewport(float width, float height);

	// Public, always accessible render settings
	bool wireframe;             // Wireframe option
	bool useProfileEffects;     // Enable or disable games current post processing effects from
	bool showSkybox;            // Enable or disable rendering of games current skybox
	bool showGizmos;            // Enable or disable scene view gizmos
	bool renderShadows;         // Enable or disable shadows

	void updateMsaaSamples(unsigned int msaaSamples);

	Camera& getFlyCamera();

private:
	void createPasses(); // Create all passes
	void destroyPasses(); // Destroy all passes

	// Scene views viewport
	Viewport viewport;

	// Render settings
	unsigned int msaaSamples;   // MSAA Samples

	// Post processing profile
	PostProcessing::Profile defaultProfile; // Default scene view profile

	// Scene view cameras
	Camera flyCamera;

	// Linked passes
	PrePass prePass;
	SceneViewForwardPass sceneViewForwardPass;
	SSAOPass ssaoPass;
	VelocityBuffer velocityBuffer;
	PostProcessingPipeline postProcessingPipeline;

	// Other instances
	IMGizmo imGizmo;
};

