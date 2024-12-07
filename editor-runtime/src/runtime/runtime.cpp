#include "runtime.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <stb_image.h>
#include <stb_image_write.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

#include "../core/rendering/shader/shader.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/material/unlit/unlit_material.h"
#include "../core/rendering/material/lit/lit_material.h"
#include "../core/rendering/texture/texture.h"
#include "../core/rendering/model/model.h"
#include "../core/rendering/core/mesh_renderer.h"
#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/primitives/quad.h"
#include "../core/entity/entity.h"
#include "../core/camera/camera.h"
#include "../core/utils/log.h"
#include "../core/diagnostics/profiler.h"
#include "../core/input/input.h"
#include "../core/rendering/core/transformation.h"
#include "../core/viewport/viewport.h"
#include "../core/rendering/skybox/cubemap.h"
#include "../core/time/time.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/input/cursor.h"

#include "../src/ui/editor_ui.h"
#include "../src/example/src/game_logic.h"

#include "../runtime/tmp_context.h"

std::vector<Entity*> Runtime::entityStack;

Skybox Runtime::defaultSkybox;

Camera Runtime::camera;

glm::vec4 Runtime::clearColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
unsigned int Runtime::msaaSamples = 4;
bool Runtime::vsync = false;
bool Runtime::sceneViewWireframe = false;
bool Runtime::solidMode = false;
bool Runtime::shadows = true;
bool Runtime::postProcessingEffects = true;
Skybox& Runtime::currentSkybox = Runtime::defaultSkybox;

GLFWwindow* Runtime::glfw = nullptr;
glm::vec2 Runtime::windowSize = glm::vec2(1800.0f, 1000.0f);
bool Runtime::fullscreen = true;

Viewport Runtime::sceneViewport;

ShadowDisk* Runtime::mainShadowDisk = nullptr;
ShadowMap* Runtime::mainShadowMap = nullptr;

PrePass Runtime::prePass = PrePass(Runtime::sceneViewport);
ForwardPass Runtime::forwardPass = ForwardPass(Runtime::sceneViewport);
SceneViewForwardPass Runtime::sceneViewForwardPass = SceneViewForwardPass(Runtime::sceneViewport);
SSAOPass Runtime::ssaoPass = SSAOPass(Runtime::sceneViewport);
VelocityBuffer Runtime::velocityBuffer = VelocityBuffer(Runtime::sceneViewport);
PostProcessingPipeline Runtime::postProcessingPipeline = PostProcessingPipeline(Runtime::sceneViewport, false);

PostProcessing::Profile Runtime::sceneViewProfile;
PostProcessing::Profile Runtime::gameViewProfile;

IMGizmo Runtime::quickGizmo;

SceneViewPipeline Runtime::sceneViewPipeline;

bool Runtime::resized = false;

//
//
// PUBLIC RUNTIME METHODS
//
//

Entity* Runtime::createEntity()
{
	// Create entity and push it to the entity stack
	Entity* entity = new Entity();
	entityStack.push_back(entity);

	// Return created entity
	return entity;
}

void Runtime::destroyEntity(Entity* entity) {
	// Try to find entity in entity stack
	auto it = std::find(entityStack.begin(), entityStack.end(), entity);

	if (it != entityStack.end()) {
		// Remove entity from entity stack and delete it on the heap
		entityStack.erase(it);
		delete entity;
	}
}

Camera& Runtime::getCamera()
{
	return camera;
}

//
//
// RUNTIME BASE METHODS (START & TERMINATE)
//
//

int Runtime::START_LOOP()
{
	// CREATE CONTEXT AND LOAD GRAPHICS API //
	Log::printProcessStart("Runtime", "Creating context...");
	setupGlfw(); // Setup window
	setVSync(); // Set vsync
	Log::printProcessDone("Runtime", "Context created");

	// LOAD ASSETS, COMPILE SHADERS
	loadAssets();

	// CALL ANY OTHER SCRIPTS NEEDING SETUP
	setupScripts();

	// PERFORM GAMES AWAKE LOGIC
	awake();

	while (!glfwWindowShouldClose(glfw))
	{
		// Check if window has been resized
		checkWindowResize();

		// UPDATE ANY SCRIPTS NEEDING UPDATE FOR NEXT FRAME (Time, Inputs etc.)
		prepareFrame();

		// UPDATE GAME LOGIC
		update();

		//
		// EXTERNAL TRANSFORM MANIPULATION HERE (e.g. physics)
		//

		// RENDER NEXT FRAME (full render pipeline pass)
		renderShadows();
		sceneViewPipeline.render(entityStack);
		renderGameView();

		// RENDER EDITOR
		renderEditor();

		// FINISH CURRENT FRAME
		finishFrame();
	}

	// Exit application

	glfwTerminate();
	return 0;
}

void Runtime::TERMINATE()
{
	if (glfw != nullptr)
	{
		glfwDestroyWindow(glfw);
		glfwTerminate();
	}
	std::exit(0);
}

//
//
// PRIVATE RUNTIME CORE METHODS
//
//

void Runtime::setupGlfw() {

	// Set error callback and initialize context
	glfwSetErrorCallback(glfwErrorCallback);
	glfwInit();

	// Set versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Enable HDR output
	glfwWindowHint(GLFW_RED_BITS, 10);
	glfwWindowHint(GLFW_GREEN_BITS, 10);
	glfwWindowHint(GLFW_BLUE_BITS, 10);
	glfwWindowHint(GLFW_ALPHA_BITS, 2);

	// Check for fullscreen
	if (fullscreen)
	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		windowSize = glm::vec2(mode->width, mode->height);
	}

	// Create window
	glfw = glfwCreateWindow(windowSize.x, windowSize.y, "Rendering Alpha", nullptr, nullptr);

	// Set resize callbacks
	glfwSetFramebufferSizeCallback(glfw, glfwWindowSizeCallback);
	glfwSetWindowSizeCallback(glfw, glfwWindowSizeCallback);

	if (glfw == nullptr)
	{
		Log::printError("GLFW", "Creation of window failed");
	}

	// Load graphics api
	glfwMakeContextCurrent(glfw);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log::printError("GLFW", "Initialization of GLAD failed");
	}

	// Debug graphics api version
	const char* version = (const char*)glGetString(GL_VERSION);
	Log::printProcessDone("GLFW", "Initialized, OpenGL version: " + std::string(version));
}

void Runtime::setVSync() {

	glfwSwapInterval(vsync ? 1 : 0);

}

void Runtime::loadAssets() {

	// Loading all shaders
	std::vector<std::string> shader_paths = {
		"../resources/shaders/materials",
		"../resources/shaders/postprocessing",
		"../resources/shaders/gizmo",
		"../resources/shaders/passes" };
	ShaderPool::loadAndCompile(shader_paths);

	// Create shadow disk
	unsigned int diskWindowSize = 4;
	unsigned int diskFilterSize = 8;
	unsigned int diskRadius = 5;
	mainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);

	// Create default shadow map
	bool shadow_map_saved = false;
	mainShadowMap = new ShadowMap(4096, 4096, 40.0f, 40.0f, 0.3f, 1000.0f);

	// Create default skybox
	Cubemap defaultCubemap = Cubemap::loadByCubemap("../resources/skybox/default/default_night.png");
	defaultSkybox = Skybox(defaultCubemap);

	// Set default skybox as current skybox
	currentSkybox = defaultSkybox;

	// Load gizmo icons
	GizmoIconPool::loadAll("../resources/gizmos");
}

void Runtime::setupScripts() {

	// Set context for scripts needed window context
	Input::setContext(glfw);
	Cursor::setContext(glfw);

	// Setup scene view pipeline
	sceneViewPipeline.setup();

	//
	//
	// Create forward pass
	forwardPass.create(msaaSamples);
	forwardPass.setSkybox(&currentSkybox);
	forwardPass.enableQuickGizmo(&quickGizmo);

	// Create scene view forward pass
	sceneViewForwardPass.create(msaaSamples);

	// Create pre pass
	prePass.create();

	// Setup ambient occlusion pass
	ssaoPass.create();

	// Setup velocity buffer
	velocityBuffer.create();

	// Setup post processing
	postProcessingPipeline.create();

	// Setup quick gizmo
	quickGizmo.setup();
	//
	//


	// Create primitives
	Quad::create();

	// Setup engine ui
	EditorUI::setup();

}

void Runtime::prepareFrame() {

	// Update time
	Time::step(glfwGetTime());

	// Update diagnostics
	Diagnostics::step();

	// Start new frame for quick gizmos
	quickGizmo.newFrame();

	// Update input system
	Input::step();

	// Clear frame color
	glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

}

void Runtime::renderShadows()
{
	// WARNING
	// 
	// Right now, the shadow passes will use the "current model matrix" from themesh renderer.
	// As the shadows will be rendered before the other rendering pipelines,
	// which prepare the mesh renderer by among other things calculating and 
	// settings its "current model matrix", the "current model matrix" will always be
	// one frame delayed, ultimately delaying object movement for shadows by one frame
	//
	// Needs fix
	
	//
	// SHADOW PASS
	// Render shadow map
	//
	Profiler::start("shadow_pass");
	mainShadowMap->render(entityStack);
	Profiler::stop("shadow_pass");
}

void Runtime::renderGameView() {

	Profiler::start("render");

	// Get transformation matrices
	glm::mat4 viewMatrix = Transformation::viewMatrix(camera);
	glm::mat4 projectionMatrix = Transformation::projectionMatrix(camera, sceneViewport);
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix)));

	// Set mesh renderers transformation matrix caches for upcomming render passes
	MeshRenderer::currentViewMatrix = viewMatrix;
	MeshRenderer::currentProjectionMatrix = projectionMatrix;
	MeshRenderer::currentViewProjectionMatrix = viewProjectionMatrix;
	MeshRenderer::currentViewNormalMatrix = viewNormalMatrix;

	// Update cameras frustum
	camera.updateFrustum(viewProjectionMatrix);

	//
	// PREPARATION PASS
	// Prepare each mesh for upcoming render passes
	//
	for (int i = 0; i < entityStack.size(); i++)
	{
		// Could be moved to existing iteration over entity links within some pass to avoid additional iteration overhead
		// Here for now to ensure preparation despite further pipeline changes
		entityStack[i]->meshRenderer.prepareNextFrame(camera);
	}

	//
	// PRE PASS
	// Create geometry pass with depth buffer before forward pass
	//
	Profiler::start("pre_pass");
	prePass.render(entityStack);
	Profiler::stop("pre_pass");
	const unsigned int PRE_PASS_DEPTH_OUTPUT = prePass.getDepthOutput();
	const unsigned int PRE_PASS_NORMAL_OUTPUT = prePass.getNormalOutput();

	//
	// SCREEN SPACE AMBIENT OCCLUSION PASS
	// Calculate screen space ambient occlusion if enabled
	//
	Profiler::start("ssao");
	unsigned int _ssaoOutput = 0;
	if (gameViewProfile.ambientOcclusion.enabled)
	{
		_ssaoOutput = ssaoPass.render(gameViewProfile, PRE_PASS_DEPTH_OUTPUT, PRE_PASS_NORMAL_OUTPUT);
	}
	const unsigned int SSAO_OUTPUT = _ssaoOutput;
	Profiler::stop("ssao");

	//
	// VELOCITY BUFFER RENDER PASS
	//
	Profiler::start("velocity_buffer");
	const unsigned int VELOCITY_BUFFER_OUTPUT = velocityBuffer.render(gameViewProfile, entityStack);
	Profiler::stop("velocity_buffer");

	//
	// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
	// Includes injected pre pass
	//

	// Prepare lit material with current render data
	LitMaterial::viewport = &sceneViewport; // Redundant most of the times atm
	LitMaterial::camera = &camera; // Redundant most of the times atm
	LitMaterial::ssaoInput = SSAO_OUTPUT;
	LitMaterial::profile = &gameViewProfile;
	LitMaterial::castShadows = true;
	LitMaterial::mainShadowDisk = mainShadowDisk;
	LitMaterial::mainShadowMap = mainShadowMap;

	Profiler::start("forward_pass");
	unsigned int FORWARD_PASS_OUTPUT = forwardPass.render(entityStack);
	Profiler::stop("forward_pass");

	//
	// POST PROCESSING PASS
	// Render post processing pass to screen using forward pass output as input
	//
	Profiler::start("post_processing");
	postProcessingPipeline.render(gameViewProfile, FORWARD_PASS_OUTPUT, PRE_PASS_DEPTH_OUTPUT, VELOCITY_BUFFER_OUTPUT);
	Profiler::stop("post_processing");

	Profiler::stop("render");

}

void Runtime::renderEditor() {

	Profiler::start("ui_pass");

	EditorUI::newFrame();
	EditorUI::render();

	Profiler::stop("ui_pass");

}

void Runtime::finishFrame() {

	// Sqap glfw buffers and poll events
	glfwSwapBuffers(glfw);
	glfwPollEvents();

}

void Runtime::checkWindowResize()
{
	if (!resized) return;
	performResize();
	resized = false;
}

void Runtime::resizeViewport(float width, float height) {
	// TMP ALWAYS USE SCENE VIEWPORT
	sceneViewport.width = width;
	sceneViewport.height = height;
	performResize();
}

void Runtime::performResize() {

	// Destroy all passes/pipelines which are bound to a fixed viewport size
	forwardPass.destroy();
	sceneViewForwardPass.destroy();
	prePass.destroy();
	ssaoPass.destroy();
	velocityBuffer.destroy();
	postProcessingPipeline.destroy();

	// Recreate all destroyed passes/pipelines
	forwardPass.create(msaaSamples);
	sceneViewForwardPass.create(msaaSamples);
	prePass.create();
	ssaoPass.create();
	velocityBuffer.create();
	postProcessingPipeline.create();

	Log::printProcessDone("Context", "Resize operation performed, various viewport dependant passes recreated");
}

void Runtime::glfwErrorCallback(int error, const char* description)
{

	Log::printError("GLFW", "Error: " + std::to_string(error), description);

}

void Runtime::glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {

	sceneViewport.width = width;
	sceneViewport.height = height;

	resized = true;

}