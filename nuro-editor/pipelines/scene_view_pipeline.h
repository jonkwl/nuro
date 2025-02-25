#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <viewport/viewport.h>
#include <ecs/ecs_collection.h>
#include <rendering/skybox/skybox.h>
#include <rendering/gizmos/gizmos.h>
#include <rendering/passes/ssao_pass.h>
#include <rendering/passes/pre_pass.h>
#include <rendering/passes/preprocessor_pass.h>
#include <rendering/velocitybuffer/velocity_buffer.h>
#include <rendering/postprocessing/post_processing.h>
#include <rendering/sceneview/scene_view_forward_pass.h>
#include <rendering/postprocessing/post_processing_pipeline.h>

class SceneViewPipeline
{
public:
	SceneViewPipeline();

	void create(); // Create scene view pipeline
	void destroy(); // Destroy scene view pipeline

	// Renders scene view
	void render();

	// Returns color output from latest render
	uint32_t getOutput();

	// Returns normal output from latest pre pass render
	uint32_t getPrePassNormals();

	// Returns depth output from latest pre pass render
	uint32_t getPrePassDepth();

	// Returns the viewport used
	const Viewport& getViewport();

	// Resizes the viewport used
	void resizeViewport(float width, float height);
	
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

	// Updates the msaa samples of the scene view
	void updateMsaaSamples(uint32_t msaaSamples);

	// Sets the currently selected entity
	void setSelectedEntity(EntityContainer* selected);

	// Unselects all selected entitites
	void unselectEntities();

	// Returns the selected entities
	const std::vector<EntityContainer*>& getSelectedEntities() const;

	// Returns the scene views fly camera
	Camera& getFlyCamera();

	// Latest view matrix used
	const glm::mat4& getView() const;

	// Latest projection matrix used
	const glm::mat4& getProjection() const;

private:
	// Creates all passes
	void createPasses();

	// Destroys all passes
	void destroyPasses();

	// Scene views viewport
	Viewport viewport;

	//
	// Render settings
	//

	uint32_t msaaSamples;
	PostProcessing::Profile defaultProfile;

	//
	// Scene view cameras
	//

	TransformComponent flyCameraTransform;
	CameraComponent flyCameraRoot;
	Camera flyCamera;

	//
	// Linked passes
	//

	PreprocessorPass preprocessorPass;
	PrePass prePass;
	SceneViewForwardPass sceneViewForwardPass;
	SSAOPass ssaoPass;
	PostProcessingPipeline postProcessingPipeline;

	//
	// Matrix cache
	//

	glm::mat4 view;
	glm::mat4 projection;

	//
	// Entity selection
	//

	std::vector<EntityContainer*> selectedEntities;

	//
	// States
	//

	// If scene view was rendered at least once
	bool frameInitialized;

	// Render count for initial frames
	uint32_t initialRenderCount;
};