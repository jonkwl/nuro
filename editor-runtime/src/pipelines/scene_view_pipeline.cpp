#include "scene_view_pipeline.h"

#include "../core/input/input.h"
#include "../core/rendering/core/transformation.h"
#include "../core/utils/log.h"
#include "../core/diagnostics/profiler.h"
#include "../core/rendering/material/lit/lit_material.h"
#include "../core/rendering/culling/bounding_volume.h"
#include "../core/rendering/shadows/shadow_map.h"

#include "../src/ui/windows/scene_window.h"
#include "../src/runtime/runtime.h"

// initialize with users editor settings later
SceneViewPipeline::SceneViewPipeline() : wireframe(false),
useProfileEffects(false),
showSkybox(false),
showGizmos(false),
renderShadows(false),
viewport(),
msaaSamples(8), // should lower this
defaultProfile(),
flyCameraTransform(),
flyCameraRoot(),
flyCamera(flyCameraTransform, flyCameraRoot),
prePass(viewport),
sceneViewForwardPass(viewport),
ssaoPass(viewport),
postProcessingPipeline(viewport, false),
imGizmo(),
view(glm::mat4(1.0f)),
projection(glm::mat4(1.0f))
{
}

void SceneViewPipeline::setup()
{
	// Setup quick gizmo
	imGizmo.setup();

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

	// Create passes
	createPasses();
}

void SceneViewPipeline::render()
{
	Profiler::start("render");

	// Start new frame for quick gizmos
	imGizmo.newFrame();

	// Pick variable items for rendering
	Camera& targetCamera = flyCamera;

	// Select game profile if profile effects are enabled and not rendering wireframe
	bool useDefaultProfile = !useProfileEffects || wireframe;
	PostProcessing::Profile& targetProfile = useDefaultProfile ? defaultProfile : Runtime::gameViewPipeline.getProfile();

	// Get transformation matrices
	view = Transformation::view(targetCamera.transform.position, targetCamera.transform.rotation);
	projection = Transformation::projection(targetCamera.root.fov, targetCamera.root.near, targetCamera.root.far, viewport);
	glm::mat4 viewProjection = projection * view;
	glm::mat3 viewNormal = glm::transpose(glm::inverse(glm::mat3(view)));

	// Render test light gizmo
	imGizmo.color = GizmoColor::BLUE;
	imGizmo.foreground = false;
	imGizmo.opacity = 0.08f;
	imGizmo.icon3d(GizmoIconPool::get("fa_lightbulb"), glm::vec3(0.0f, 0.0f, 6.5f), targetCamera.transform, glm::vec3(0.35f));

	//
	// PRE PASS
	// Create geometry pass with depth buffer before forward pass
	//
	Profiler::start("pre_pass");
	prePass.render(viewProjection, viewNormal);
	Profiler::stop("pre_pass");
	const uint32_t PRE_PASS_DEPTH_OUTPUT = prePass.getDepthOutput();
	const uint32_t PRE_PASS_NORMAL_OUTPUT = prePass.getNormalOutput();

	//
	// SCREEN SPACE AMBIENT OCCLUSION PASS
	// Calculate screen space ambient occlusion if enabled
	//
	Profiler::start("ssao");
	uint32_t _ssaoOutput = 0;
	if (targetProfile.ambientOcclusion.enabled)
	{
		_ssaoOutput = ssaoPass.render(projection, targetProfile, PRE_PASS_DEPTH_OUTPUT, PRE_PASS_NORMAL_OUTPUT);
	}
	const uint32_t SSAO_OUTPUT = _ssaoOutput;
	Profiler::stop("ssao");

	//
	// VELOCITY BUFFER RENDER PASS (NONE)
	//
	const uint32_t VELOCITY_BUFFER_OUTPUT = 0;

	//
	// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
	//

	// Prepare lit material with current render data
	LitMaterial::viewport = &viewport; // Redundant most of the times atm
	LitMaterial::cameraTransform = &targetCamera.transform; // Redundant most of the times atm
	LitMaterial::ssaoInput = SSAO_OUTPUT;
	LitMaterial::profile = &targetProfile;
	LitMaterial::castShadows = renderShadows;
	LitMaterial::mainShadowDisk = Runtime::mainShadowDisk;
	LitMaterial::mainShadowMap = Runtime::mainShadowMap;
	LitMaterial::lightSpace = Runtime::mainShadowMap->getLightSpace();

	Profiler::start("forward_pass");
	sceneViewForwardPass.wireframe = wireframe;
	sceneViewForwardPass.drawSkybox = showSkybox;
	sceneViewForwardPass.setSkybox(Runtime::gameViewPipeline.getSkybox());
	sceneViewForwardPass.drawQuickGizmos = showGizmos;
	uint32_t FORWARD_PASS_OUTPUT = sceneViewForwardPass.render(view, projection, viewProjection, 0);
	Profiler::stop("forward_pass");

	//
	// POST PROCESSING PASS
	// Render post processing pass to screen using forward pass output as input
	//
	Profiler::start("post_processing");
	postProcessingPipeline.render(view, projection, viewProjection, targetProfile, FORWARD_PASS_OUTPUT, PRE_PASS_DEPTH_OUTPUT, VELOCITY_BUFFER_OUTPUT);
	Profiler::stop("post_processing");

	Profiler::stop("render");
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
	viewport.width = width;
	viewport.height = height;

	// Recreate all passes to match new viewport
	destroyPasses();
	createPasses();

	Log::printProcessDone("Scene View", "Resize operation performed, various viewport dependant passes recreated");
}

void SceneViewPipeline::updateMsaaSamples(uint32_t _msaaSamples)
{
	// Set new msaa samples and recreate scene view forward pass
	msaaSamples = _msaaSamples;
	sceneViewForwardPass.destroy();
	sceneViewForwardPass.create(msaaSamples);
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
	sceneViewForwardPass.setQuickGizmo(&imGizmo);
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