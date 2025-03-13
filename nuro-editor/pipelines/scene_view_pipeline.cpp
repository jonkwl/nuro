#include "scene_view_pipeline.h"

#include <utils/console.h>
#include <input/input.h>
#include <physics/physics.h>
#include <diagnostics/profiler.h>
#include <rendering/shadows/shadow_map.h>
#include <rendering/transformation/transformation.h>
#include <rendering/culling/bounding_volume.h>
#include <rendering/material/lit/lit_material.h>

#include "../runtime/runtime.h"
#include "../ui/windows/viewport_window.h"
#include "../gizmos/component_gizmos.h"
#include "../gizmos/editor_gizmo_color.h"

// initialize with users editor settings later
SceneViewPipeline::SceneViewPipeline() : wireframe(false),
useProfileEffects(true),
showSkybox(false),
showGizmos(true),
renderingShadows(true),
viewport(),
msaaSamples(4),
defaultProfile(),
flyCameraTransform(),
flyCameraRoot(),
flyCamera(flyCameraTransform, flyCameraRoot),
preprocessorPass(),
prePass(viewport),
sceneViewForwardPass(viewport),
ssaoPass(viewport),
postProcessingPipeline(viewport, false),
view(glm::mat4(1.0f)),
projection(glm::mat4(1.0f)),
selectedEntities(),
frameInitialized(false),
initialRenderCount(0)
{
}

void SceneViewPipeline::create()
{
	// Initialize default profile:
	// Neutral color settings
	defaultProfile.color.exposure = 1.0f;
	defaultProfile.color.contrast = 1.0f;
	defaultProfile.color.gamma = 2.2f;
	// All effects disabled
	defaultProfile.motionBlur.enabled = false;
	defaultProfile.bloom.enabled = false;
	defaultProfile.chromaticAberration.enabled = false;
	defaultProfile.vignette.enabled = false;
	defaultProfile.ambientOcclusion.enabled = false;

	// Setup fly camera
	std::get<1>(flyCamera).fov = 90.0f;

	// Create passes
	createPasses();

	// Set game view gizmos
	Runtime::getGameViewPipeline().linkGizmos(&Runtime::getSceneGizmos());
}

void SceneViewPipeline::destroy()
{
	// Destroy passes
	destroyPasses();
}

void SceneViewPipeline::render()
{
	Profiler::start("scene_view");

	// Pick variable items for rendering
	Camera& camera = flyCamera;
	
	// Get camera bindings
	auto& [cameraTransform, cameraHandle] = camera;

	// Select game profile if profile effects are enabled and not rendering wireframe
	bool useDefaultProfile = !useProfileEffects || wireframe;
	PostProcessing::Profile& targetProfile = useDefaultProfile ? defaultProfile : Runtime::getGameViewPipeline().getProfile();

	// Get transformation matrices
	view = Transformation::view(cameraTransform.position, cameraTransform.rotation);
	projection = Transformation::projection(cameraHandle.fov, viewport.getAspect(), cameraHandle.near, cameraHandle.far);
	glm::mat4 viewProjection = projection * view;
	glm::mat3 viewNormal = glm::transpose(glm::inverse(glm::mat3(view)));

	// Start new gizmo frame
	IMGizmo& gizmos = Runtime::getSceneGizmos();
	gizmos.newFrame();
	ComponentGizmos::drawSceneViewIcons(gizmos, cameraTransform);

	//
	// PREPROCESSOR PASS
	// Evaluate and update transforms, perform culling etc.
	// 
	preprocessorPass.perform(viewProjection);

	//
	// PRE PASS
	// Create geometry pass with depth buffer before forward pass
	//
	prePass.render(viewProjection, viewNormal);
	const uint32_t PRE_PASS_DEPTH_OUTPUT = prePass.getDepthOutput();
	const uint32_t PRE_PASS_NORMAL_OUTPUT = prePass.getNormalOutput();

	//
	// SCREEN SPACE AMBIENT OCCLUSION PASS
	// Calculate screen space ambient occlusion if enabled
	//
	uint32_t _ssaoOutput = 0;
	if (targetProfile.ambientOcclusion.enabled)
	{
		_ssaoOutput = ssaoPass.render(projection, targetProfile, PRE_PASS_DEPTH_OUTPUT, PRE_PASS_NORMAL_OUTPUT);
	}
	const uint32_t SSAO_OUTPUT = _ssaoOutput;

	//
	// VELOCITY BUFFER RENDER PASS (NONE)
	//
	const uint32_t VELOCITY_BUFFER_OUTPUT = 0;

	//
	// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
	//

	// Prepare lit material with current render data
	LitMaterial::viewport = &viewport; // Redundant most of the times atm
	LitMaterial::cameraTransform = &cameraTransform; // Redundant most of the times atm
	LitMaterial::ssaoInput = SSAO_OUTPUT;
	LitMaterial::profile = &targetProfile;
	LitMaterial::castShadows = renderingShadows;
	LitMaterial::mainShadowDisk = Runtime::getMainShadowDisk();
	LitMaterial::mainShadowMap = Runtime::getMainShadowMap();

	sceneViewForwardPass.wireframe = wireframe;
	sceneViewForwardPass.drawSkybox = showSkybox;
	sceneViewForwardPass.linkSkybox(Runtime::getGameViewPipeline().getLinkedSkybox());
	sceneViewForwardPass.drawGizmos = showGizmos;
	uint32_t FORWARD_PASS_OUTPUT = sceneViewForwardPass.render(view, projection, viewProjection, camera, selectedEntities);

	//
	// POST PROCESSING PASS
	// Render post processing pass to screen using forward pass output as input
	//
	postProcessingPipeline.render(view, projection, viewProjection, targetProfile, FORWARD_PASS_OUTPUT, PRE_PASS_DEPTH_OUTPUT, VELOCITY_BUFFER_OUTPUT);

	Profiler::stop("scene_view");
}

uint32_t SceneViewPipeline::getOutput()
{
	return postProcessingPipeline.getOutput();
}

uint32_t SceneViewPipeline::getPrePassNormals()
{
	return prePass.getNormalOutput();
}

uint32_t SceneViewPipeline::getPrePassDepth()
{
	return prePass.getDepthOutput();
}

const Viewport& SceneViewPipeline::getViewport()
{
	return viewport;
}

void SceneViewPipeline::resizeViewport(float width, float height)
{
	// Set new viewport size
	viewport.resize(width, height);

	// Recreate all passes to match new viewport
	destroyPasses();
	createPasses();

	// Re-render frame
	render();
}

void SceneViewPipeline::updateMsaaSamples(uint32_t _msaaSamples)
{
	// Set new msaa samples and recreate scene view forward pass
	msaaSamples = _msaaSamples;
	sceneViewForwardPass.destroy();
	sceneViewForwardPass.create(msaaSamples);
}

void SceneViewPipeline::setSelectedEntity(EntityContainer* _selected)
{
	selectedEntities = { _selected };
}

void SceneViewPipeline::unselectEntities()
{
	selectedEntities = {};
}

const std::vector<EntityContainer*>& SceneViewPipeline::getSelectedEntities() const
{
	return selectedEntities;
}

Camera& SceneViewPipeline::getFlyCamera()
{
	return flyCamera;
}

const glm::mat4& SceneViewPipeline::getView() const
{
	return view;
}

const glm::mat4& SceneViewPipeline::getProjection() const
{
	return projection;
}

void SceneViewPipeline::createPasses()
{
	prePass.create();
	sceneViewForwardPass.create(msaaSamples);
	sceneViewForwardPass.linkGizmos(&Runtime::getSceneGizmos());
	ssaoPass.create();
	postProcessingPipeline.create();
}

void SceneViewPipeline::destroyPasses()
{
	prePass.destroy();
	sceneViewForwardPass.destroy();
	ssaoPass.destroy();
	postProcessingPipeline.destroy();
}