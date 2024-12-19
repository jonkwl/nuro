#pragma once

#include <vector>
#include <utility>
#include <glm.hpp>

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
#include "../core/ecs/components.h"

class SceneViewPipeline
{
public:
	SceneViewPipeline();

	void setup(); // Setup scene view pipeline

	void render(std::vector<OldEntity*>& targets); // Renders scene view

	unsigned int getOutput(); // Returns color output from latest render

	unsigned int getPrePassNormals(); // Returns normal output from latest pre pass render
	unsigned int getPrePassDepth(); // Returns depth output from latest pre pass render

	const Viewport& getViewport();
	void resizeViewport(float width, float height);

	// Public, always accessible render settings
	bool wireframe;             // Wireframe option
	bool useProfileEffects;     // Enable or disable games current post processing effects from
	bool showSkybox;            // Enable or disable rendering of games current skybox
	bool showGizmos;            // Enable or disable scene view gizmos
	bool renderShadows;         // Enable or disable shadows

	void updateMsaaSamples(unsigned int msaaSamples);

	std::pair<TransformComponent, CameraComponent>& getFlyCamera();

	const glm::mat4& getView() const;
	const glm::mat4& getProjection() const;

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
	std::pair<TransformComponent, CameraComponent> flyCamera;

	// Linked passes
	PrePass prePass;
	SceneViewForwardPass sceneViewForwardPass;
	SSAOPass ssaoPass;
	VelocityBuffer velocityBuffer;
	PostProcessingPipeline postProcessingPipeline;

	// Other instances
	IMGizmo imGizmo;

	// Matrix cache
	glm::mat4 view;
	glm::mat4 projection;
};

