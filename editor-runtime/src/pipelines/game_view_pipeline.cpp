#include "game_view_pipeline.h"

#include "../core/input/input.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/core/mesh_renderer.h"
#include "../core/utils/log.h"
#include "../core/diagnostics/profiler.h"
#include "../core/rendering/material/lit/lit_material.h"
#include "../core/rendering/culling/bounding_volume.h"

#include "../src/ui/windows/scene_window.h"
#include "../src/runtime/runtime.h"

// initialize with users editor settings later
GameViewPipeline::GameViewPipeline() : viewport(),
msaaSamples(8), // should lower this
profile(),
camera(),
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

	// Get transformation matrices
	glm::mat4 viewMatrix = Transformation::viewMatrix(camera);
	glm::mat4 projectionMatrix = Transformation::projectionMatrix(camera, viewport);
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix)));

	// Set mesh renderers transformation matrix caches for upcomming render passes
	MeshRenderer::currentViewMatrix = viewMatrix;
	MeshRenderer::currentProjectionMatrix = projectionMatrix;
	MeshRenderer::currentViewProjectionMatrix = viewProjectionMatrix;
	MeshRenderer::currentViewNormalMatrix = viewNormalMatrix;

	// Update cameras frustum
	camera.updateFrustum(viewport);

	//
	// PREPARATION PASS
	// Prepare each mesh for upcoming render passes
	//
	for (int i = 0; i < targets.size(); i++)
	{
		// Could be moved to existing iteration over entity links within some pass to avoid additional iteration overhead
		// Here for now to ensure preparation despite further pipeline changes
		targets[i]->meshRenderer.prepareNextFrame(camera);
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
		_ssaoOutput = ssaoPass.render(profile, PRE_PASS_DEPTH_OUTPUT, PRE_PASS_NORMAL_OUTPUT);
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
	LitMaterial::camera = &camera; // Redundant most of the times atm
	LitMaterial::ssaoInput = SSAO_OUTPUT;
	LitMaterial::profile = &profile;
	LitMaterial::castShadows = true;
	LitMaterial::mainShadowDisk = Runtime::mainShadowDisk;
	LitMaterial::mainShadowMap = Runtime::mainShadowMap;

	Profiler::start("forward_pass");
	unsigned int FORWARD_PASS_OUTPUT = forwardPass.render(targets);
	Profiler::stop("forward_pass");

	//
	// POST PROCESSING PASS
	// Render post processing pass to screen using forward pass output as input
	//
	Profiler::start("post_processing");
	postProcessingPipeline.render(profile, FORWARD_PASS_OUTPUT, PRE_PASS_DEPTH_OUTPUT, VELOCITY_BUFFER_OUTPUT);
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