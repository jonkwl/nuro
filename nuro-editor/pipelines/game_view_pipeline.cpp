#include "game_view_pipeline.h"

#include <input/input.h>
#include <rendering/transformation/transformation.h>
#include <utils/console.h>
#include <diagnostics/profiler.h>
#include <rendering/material/lit/lit_material.h>
#include <rendering/culling/bounding_volume.h>
#include <rendering/skybox/skybox.h>
#include <ecs/ecs_collection.h>
#include <rendering/shadows/shadow_map.h>

#include "../ui/windows/viewport_window.h"
#include "../runtime/runtime.h"

// initialize with users editor settings later
GameViewPipeline::GameViewPipeline() : drawSkybox(true),
drawGizmos(false),
viewport(),
msaaSamples(4),
profile(),
skybox(nullptr),
gizmos(nullptr),
preprocessorPass(),
prePass(viewport),
forwardPass(viewport),
ssaoPass(viewport),
velocityBuffer(viewport),
postProcessingPipeline(viewport, false),
cameraAvailable(false),
ssaoOutput(0),
velocityOutput(0)
{
}

void GameViewPipeline::create()
{
	// Create passes
	createPasses();
}

void GameViewPipeline::destroy()
{
	// Destroy passes
	destroyPasses();
}

void GameViewPipeline::render()
{
	Profiler::start("render");

	// Get active camera
	auto _camera = ECS::getLatestCamera();
	if (!_camera) {
		cameraAvailable = false;
		return;
	}
	cameraAvailable = true;
	Camera& camera = *_camera;
	auto& [cameraTransform, cameraHandle] = camera;

	// Get transformation matrices
	glm::mat4 view = Transformation::view(cameraTransform.position, cameraTransform.rotation);
	glm::mat4 projection = Transformation::projection(cameraHandle.fov, viewport.getAspect(), cameraHandle.near, cameraHandle.far);
	glm::mat4 viewProjection = projection * view;
	glm::mat3 viewNormal = glm::transpose(glm::inverse(glm::mat3(view)));

	//
	// PREPROCESSOR PASS
	// Evaluate and update transforms, perform culling etc.
	// 
	Profiler::start("preprocessor_pass");
	preprocessorPass.perform(viewProjection);
	Profiler::stop("preprocessor_pass");

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
	bool ssaoNeeded = profile.ambientOcclusion.enabled;
	ssaoOutput = 0;
	if (ssaoNeeded)
	{
		ssaoOutput = ssaoPass.render(projection, profile, PRE_PASS_DEPTH_OUTPUT, PRE_PASS_NORMAL_OUTPUT);
	}
	const uint32_t SSAO_OUTPUT = ssaoOutput;
	Profiler::stop("ssao");

	//
	// VELOCITY BUFFER RENDER PASS
	//
	Profiler::start("velocity_buffer");
	bool velocityBufferNeeded = profile.motionBlur.objectEnabled;
	velocityOutput = 0;

	if (velocityBufferNeeded) {
		velocityOutput = velocityBuffer.render(view, projection, profile);
	}

	const uint32_t VELOCITY_BUFFER_OUTPUT = velocityOutput;
	Profiler::stop("velocity_buffer");

	//
	// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
	//

	// Prepare lit material with current render data
	LitMaterial::viewport = &viewport; // Redundant most of the times atm
	LitMaterial::cameraTransform = &cameraTransform; // Redundant most of the times atm
	LitMaterial::ssaoInput = SSAO_OUTPUT;
	LitMaterial::profile = &profile;
	LitMaterial::castShadows = true;
	LitMaterial::mainShadowDisk = Runtime::getMainShadowDisk();
	LitMaterial::mainShadowMap = Runtime::getMainShadowMap();

	Profiler::start("forward_pass");
	forwardPass.drawSkybox = drawSkybox;
	forwardPass.drawGizmos = drawGizmos && gizmos;
	if (forwardPass.drawGizmos) forwardPass.linkGizmos(gizmos);
	uint32_t FORWARD_PASS_OUTPUT = forwardPass.render(view, projection, viewProjection);
	Profiler::stop("forward_pass");

	//
	// POST PROCESSING PASS
	// Render post processing pass to screen using forward pass output as input
	//
	Profiler::start("post_processing");
	postProcessingPipeline.render(view, projection, viewProjection, profile, FORWARD_PASS_OUTPUT, PRE_PASS_DEPTH_OUTPUT, VELOCITY_BUFFER_OUTPUT);
	Profiler::stop("post_processing");

	Profiler::stop("render");
}

uint32_t GameViewPipeline::getOutput()
{
	return postProcessingPipeline.getOutput();
}

const Viewport& GameViewPipeline::getViewport() const
{
	return viewport;
}

void GameViewPipeline::resizeViewport(float width, float height)
{
	// Set new viewport size
	viewport.resize(width, height);

	// Recreate all passes to match new viewport
	destroyPasses();
	createPasses();

	// Re-render frame
	render();
}

void GameViewPipeline::updateMsaaSamples(uint32_t _msaaSamples)
{
	// Set new msaa samples and recreate scene view forward pass
	msaaSamples = _msaaSamples;
	forwardPass.destroy();
	forwardPass.create(msaaSamples);
}

PostProcessing::Profile& GameViewPipeline::getProfile()
{
	return profile;
}

void GameViewPipeline::linkSkybox(Skybox* _skybox)
{
	skybox = _skybox;
	forwardPass.linkSkybox(skybox);
}

Skybox* GameViewPipeline::getLinkedSkybox()
{
	return skybox;
}

void GameViewPipeline::linkGizmos(IMGizmo* _gizmos)
{
	gizmos = _gizmos;
}

bool GameViewPipeline::getCameraAvailable()
{
	return cameraAvailable;
}

uint32_t GameViewPipeline::getSSAOOutput() const
{
	return ssaoOutput;
}

uint32_t GameViewPipeline::getVelocityOutput() const
{
	return velocityOutput;
}

void GameViewPipeline::createPasses()
{
	prePass.create();
	forwardPass.create(msaaSamples);
	ssaoPass.create();
	velocityBuffer.create();
	postProcessingPipeline.create();
}

void GameViewPipeline::destroyPasses()
{
	prePass.destroy();
	forwardPass.destroy();
	ssaoPass.destroy();
	velocityBuffer.destroy();
	postProcessingPipeline.destroy();
}