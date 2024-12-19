#include "game_view_pipeline.h"

#include "../core/input/input.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/core/mesh_renderer.h"
#include "../core/utils/log.h"
#include "../core/diagnostics/profiler.h"
#include "../core/rendering/material/lit/lit_material.h"
#include "../core/rendering/culling/bounding_volume.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/ecs/ecs.h"

#include "../src/ui/windows/scene_window.h"
#include "../src/runtime/runtime.h"

// initialize with users editor settings later
GameViewPipeline::GameViewPipeline() : viewport(),
msaaSamples(8), // should lower this
profile(),
skybox(nullptr),
prePass(viewport),
forwardPass(viewport),
ssaoPass(viewport),
velocityBuffer(viewport),
postProcessingPipeline(viewport, false)
{
}

void GameViewPipeline::setup()
{
	// Create passes
	createPasses();
}

void GameViewPipeline::render(std::vector<OldEntity*>& targets)
{
	Profiler::start("render");

	// Get active camera
	auto _camera = ECS::getActiveCamera();
	if (!_camera) {
		cameraAvailable = false;
		return;
	}
	cameraAvailable = true;
	TransformComponent& cameraTransform = std::get<0>(*_camera);
	CameraComponent& cameraComponent = std::get<1>(*_camera);

	// Get transformation matrices
	glm::mat4 view = Transformation::view(cameraTransform.position, cameraTransform.rotation);
	glm::mat4 projection = Transformation::projection(cameraComponent.fov, cameraComponent.near, cameraComponent.far, viewport);
	glm::mat4 viewProjection = projection * view;
	glm::mat3 viewNormal = glm::transpose(glm::inverse(glm::mat3(view)));

	//
	// PREPARATION PASS
	// Prepare each mesh for upcoming render passes
	//
	for (int i = 0; i < targets.size(); i++)
	{
		// Could be moved to existing iteration over entity links within some pass to avoid additional iteration overhead
		// Here for now to ensure preparation despite further pipeline changes
		targets[i]->meshRenderer.prepareNextFrame();
	}

	//
	// PRE PASS
	// Create geometry pass with depth buffer before forward pass
	//
	Profiler::start("pre_pass");
	prePass.render(targets);
	Profiler::stop("pre_pass");
	const unsigned int PRE_PASS_DEPTH_OUTPUT = prePass.getDepthOutput();
	const unsigned int PRE_PASS_NORMAL_OUTPUT = prePass.getNormalOutput();

	//
	// SCREEN SPACE AMBIENT OCCLUSION PASS
	// Calculate screen space ambient occlusion if enabled
	//
	Profiler::start("ssao");
	unsigned int _ssaoOutput = 0;
	if (profile.ambientOcclusion.enabled)
	{
		_ssaoOutput = ssaoPass.render(projection, profile, PRE_PASS_DEPTH_OUTPUT, PRE_PASS_NORMAL_OUTPUT);
	}
	const unsigned int SSAO_OUTPUT = _ssaoOutput;
	Profiler::stop("ssao");

	//
	// VELOCITY BUFFER RENDER PASS
	//
	Profiler::start("velocity_buffer");
	const unsigned int VELOCITY_BUFFER_OUTPUT = velocityBuffer.render(profile, targets);
	Profiler::stop("velocity_buffer");

	//
	// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
	// Includes injected pre pass
	//

	// Prepare lit material with current render data
	LitMaterial::viewport = &viewport; // Redundant most of the times atm
	LitMaterial::cameraTransform = &cameraTransform; // Redundant most of the times atm
	LitMaterial::ssaoInput = SSAO_OUTPUT;
	LitMaterial::profile = &profile;
	LitMaterial::castShadows = true;
	LitMaterial::mainShadowDisk = Runtime::mainShadowDisk;
	LitMaterial::mainShadowMap = Runtime::mainShadowMap;

	Profiler::start("forward_pass");
	unsigned int FORWARD_PASS_OUTPUT = forwardPass.render(targets, view, projection, viewProjection);
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

unsigned int GameViewPipeline::getOutput()
{
	return postProcessingPipeline.getOutput();
}

const Viewport& GameViewPipeline::getViewport()
{
	return viewport;
}

void GameViewPipeline::resizeViewport(float width, float height)
{
	// Set new viewport size
	viewport.width = width;
	viewport.height = height;

	// Recreate all passes to match new viewport
	destroyPasses();
	createPasses();

	Log::printProcessDone("Scene View", "Resize operation performed, various viewport dependant passes recreated");
}

void GameViewPipeline::updateMsaaSamples(unsigned int _msaaSamples)
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

void GameViewPipeline::setSkybox(Skybox* _skybox)
{
	skybox = _skybox;
	forwardPass.setSkybox(skybox);
}

Skybox* GameViewPipeline::getSkybox()
{
	return skybox;
}

bool GameViewPipeline::getCameraAvailable()
{
	return cameraAvailable;
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