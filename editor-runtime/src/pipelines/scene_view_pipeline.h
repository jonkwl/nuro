#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"
#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/sceneview/scene_view_forward_pass.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/rendering/gizmos/gizmos.h"
#include "../core/ecs/components.h"
#include "../core/ecs/composed.h"

class SceneViewPipeline
{
public:
	SceneViewPipeline();

	void setup(); // Setup scene view pipeline

	void tryRender(); // Renders scene view if conditions are met

	uint32_t getOutput(); // Returns color output from latest render

	uint32_t getPrePassNormals(); // Returns normal output from latest pre pass render
	uint32_t getPrePassDepth(); // Returns depth output from latest pre pass render

	const Viewport& getViewport();
	void resizeViewport(float width, float height);

	// Public, always accessible render settings
	bool wireframe; // Wireframe option
	bool useProfileEffects; // Enable or disable games current post processing effects from
	bool showSkybox; // Enable or disable rendering of games current skybox
	bool showGizmos; // Enable or disable scene view gizmos
	bool renderShadows; // Enable or disable shadows
	bool alwaysUpdate; // If set rendering is never skipped

	void updateMsaaSamples(uint32_t msaaSamples);

	Camera& getFlyCamera();

	const glm::mat4& getView() const;
	const glm::mat4& getProjection() const;

	void setUpdated();

private:
	void render(); // Renders scene view

	void createPasses(); // Create all passes
	void destroyPasses(); // Destroy all passes

	// Scene views viewport
	Viewport viewport;

	// Render settings
	uint32_t msaaSamples;   // MSAA Samples

	// Post processing profile
	PostProcessing::Profile defaultProfile; // Default scene view profile

	// Scene view cameras
	TransformComponent flyCameraTransform;
	CameraComponent flyCameraRoot;
	Camera flyCamera;

	// Linked passes
	PrePass prePass;
	SceneViewForwardPass sceneViewForwardPass;
	SSAOPass ssaoPass;
	PostProcessingPipeline postProcessingPipeline;

	// Other instances
	IMGizmo imGizmo;

	// Matrix cache
	glm::mat4 view;
	glm::mat4 projection;

	// States
	bool frameInitialized; // If scene view was rendered at least once
	unsigned int initialRenderCount; // Render count for initial frames
	bool updated; // If scene view was updated and therefore should be rerendered
};