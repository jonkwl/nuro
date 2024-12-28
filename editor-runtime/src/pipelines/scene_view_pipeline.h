#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/rendering/gizmos/gizmos.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/rendering/sceneview/scene_view_forward_pass.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"

class SceneViewPipeline
{
public:
	SceneViewPipeline();

	void create(); // Create scene view pipeline
	void destroy(); // Destroy scene view pipeline

	void tryRender(); // Renders scene view if conditions are met

	uint32_t getOutput(); // Returns color output from latest render

	uint32_t getPrePassNormals(); // Returns normal output from latest pre pass render
	uint32_t getPrePassDepth(); // Returns depth output from latest pre pass render

	const Viewport& getViewport();
	void resizeViewport(float width, float height);

	// Public, always accessible render settings
	
	// Wireframe option
	bool wireframe;

	// Enable or disable games current post processing effects from
	bool useProfileEffects;

	// Enable or disable rendering of games current skybox
	bool showSkybox;

	// Enable or disable scene view gizmos
	bool showGizmos;

	// Enable or disable shadows
	bool renderShadows;

	// Rendering is never skipped if set
	bool alwaysUpdate;

	// Updates the msaa samples of the scene view
	void updateMsaaSamples(uint32_t msaaSamples);

	// Sets the currently selected entity
	void setSelectedEntity(entt::entity selected);

	// Unselects all selected entitites
	void unselectEntities();

	// Returns the selected entities
	const std::vector<entt::entity>& getSelectedEntities() const;

	// Returns the scene views fly camera
	Camera& getFlyCamera();

	// Latest view matrix used
	const glm::mat4& getView() const;

	// Latest projection matrix used
	const glm::mat4& getProjection() const;

	// Sets the scene views state to updated, so it will surely re-render the next frame
	void setUpdated();

private:
	// Renders scene view
	void render();

	// Creates all passes
	void createPasses();

	// Destroys all passes
	void destroyPasses();

	// Scene views viewport

	Viewport viewport;

	// Render settings

	uint32_t msaaSamples;

	// Post processing profile
	PostProcessing::Profile defaultProfile;

	// Scene view cameras

	TransformComponent flyCameraTransform;
	CameraComponent flyCameraRoot;
	Camera flyCamera;

	// Linked passes

	PrePass prePass;
	SceneViewForwardPass sceneViewForwardPass;
	SSAOPass ssaoPass;
	PostProcessingPipeline postProcessingPipeline;

	// Matrix cache

	glm::mat4 view;
	glm::mat4 projection;

	// Entity selection
	
	std::vector<entt::entity> selectedEntities;

	// States

	bool frameInitialized; // If scene view was rendered at least once
	uint32_t initialRenderCount; // Render count for initial frames
	bool updated; // If scene view was updated and therefore should be rerendered
};