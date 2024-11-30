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

#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/model/model.h"
#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/shadows/shadow_map.h"
#include "../src/rendering/shadows/shadow_disk.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/entity/entity.h"
#include "../src/camera/camera.h"
#include "../src/utils/log.h"
#include "../src/utils/inspector_mode.h"
#include "../src/utils/profiler.h"
#include "../src/editor/engine_ui.h"
#include "../src/input/input.h"
#include "../src/rendering/gizmos/gizmos.h"
#include "../user/src/game_logic.h"
#include "../src/rendering/core/transformation.h"
#include "../src/viewport/viewport.h"
#include "../src/rendering/skybox/cubemap.h"

std::vector<Entity*> Runtime::entityStack;

UnlitMaterial* Runtime::defaultMaterial = nullptr;
Skybox Runtime::defaultSkybox;

Shader* Runtime::prePassShader = nullptr;
Shader* Runtime::shadowPassShader = nullptr;
Shader* Runtime::velocityPassShader = nullptr;

Camera Runtime::renderCamera;
Camera Runtime::activeCamera;
Camera Runtime::inspectorCamera;

float Runtime::time = 0.0f;
float Runtime::lastTime = 0.0f;
float Runtime::deltaTime = 0.0f;
float Runtime::fps = 0;
float Runtime::averageFps = 0.0f;

glm::vec4 Runtime::clearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
unsigned int Runtime::msaaSamples = 4;
bool Runtime::vsync = false;
bool Runtime::wireframe = false;
bool Runtime::solidMode = false;
bool Runtime::shadows = true;
bool Runtime::postProcessingEffects = true;

bool Runtime::inspectorMode = true;
bool Runtime::showEngineUI = false;
bool Runtime::showDiagnostics = true;

bool Runtime::skyboxEnabled = true;
Skybox& Runtime::selectedSkybox = Runtime::defaultSkybox;

GLFWwindow* Runtime::glfw = nullptr;
glm::vec2 Runtime::windowSize = glm::vec2(800.0f, 400.0f);
bool Runtime::fullscreen = false;
GLenum Runtime::cursorMode = GLFW_CURSOR_DISABLED;

Viewport Runtime::sceneViewport;

ShadowDisk* Runtime::mainShadowDisk = nullptr;
ShadowMap* Runtime::mainShadowMap = nullptr;

unsigned int Runtime::currentDrawCalls = 0;
unsigned int Runtime::currentVertices = 0;
unsigned int Runtime::currentPolygons = 0;

float Runtime::directionalIntensity = 0.1f;
glm::vec3 Runtime::directionalColor = glm::vec3(0.8f, 0.8f, 1.0f);
glm::vec3 Runtime::directionalDirection = glm::vec3(-0.7f, -0.8f, 1.0f);
glm::vec3 Runtime::directionalPosition = glm::vec3(4.0f, 5.0f, -7.0f);

float Runtime::intensity = 0.5f;
float Runtime::range = 15.0f;
float Runtime::falloff = 7.5f;
bool Runtime::normalMapping = true;
float Runtime::normalMappingIntensity = 1.0f;
unsigned int Runtime::nCPUEntities = 0;
unsigned int Runtime::nGPUEntities = 0;

PrePass Runtime::prePass = PrePass(Runtime::sceneViewport);
ForwardPass Runtime::forwardPass = ForwardPass(Runtime::sceneViewport);
SSAOPass Runtime::ssaoPass = SSAOPass(Runtime::sceneViewport);
PostProcessingPipeline Runtime::postProcessingPipeline = PostProcessingPipeline(Runtime::sceneViewport);

unsigned int Runtime::prePassDepthOutput = 0;
unsigned int Runtime::prePassNormalOutput = 0;
unsigned int Runtime::ssaoBuffer = 0;

bool skipSkyboxLoad = false; // tmp

int Runtime::averageFpsFrameCount = 0;
float Runtime::averageFpsElapsedTime = 0.0f;

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

void Runtime::useCamera(Camera camera)
{
	activeCamera = camera;
}

Camera& Runtime::getCameraRendering()
{
	return renderCamera;
}

Camera& Runtime::getActiveCamera()
{
	return activeCamera;
}

Camera& Runtime::getInspectorCamera()
{
	return inspectorCamera;
}

void Runtime::setCursor(GLenum cursorMode)
{
	Runtime::cursorMode = cursorMode;
	glfwSetInputMode(Runtime::glfw, GLFW_CURSOR, cursorMode);
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
		checkResize();

		// PREPARE INTERNAL VARIABLES FOR NEXT FRAME (Time, Delta Time etc.)
		prepareFrameInternal();

		// UPDATE ANY SCRIPTS NEEDING UPDATE FOR NEXT FRAME (Input system update etc.)
		prepareFrameExternal();

		//
		// EXTERNAL TRANSFORM MANIPULATION HERE (e.g. physics)
		//

		// UPDATE GAME LOGIC
		update();

		// RENDER NEXT FRAME (full render pipeline pass)
		renderFrame();

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

	// Set defualt window cursor
	setCursor(cursorMode);

	// Set scene viewport
	sceneViewport.width = windowSize.x;
	sceneViewport.height = windowSize.y;
}

void Runtime::setVSync() {

	glfwSwapInterval(vsync ? 1 : 0);

}

void Runtime::loadAssets() {

	// Loading all shaders
	std::vector<std::string> shader_paths = {
		"./resources/shaders/materials",
		"./resources/shaders/postprocessing",
		"./resources/shaders/gizmo",
		"./resources/shaders/passes" };
	ShaderPool::loadAndCompile(shader_paths);

	prePassShader = ShaderPool::get("pre_pass");
	shadowPassShader = ShaderPool::get("shadow_pass");
	velocityPassShader = ShaderPool::get("velocity_pass");

	// Creating default material
	defaultMaterial = new UnlitMaterial();
	defaultMaterial->baseColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	// Create shadow disk
	unsigned int diskWindowSize = 4;
	unsigned int diskFilterSize = 8;
	unsigned int diskRadius = 5;
	mainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);

	// Create default shadow map
	bool shadow_map_saved = false;
	mainShadowMap = new ShadowMap(4096, 4096, 40.0f, 40.0f);

	// Creating default skybox
	if (!skipSkyboxLoad)
	{
		// Create default skybox
		Cubemap defaultCubemap = Cubemap::loadByCubemap("./resources/skybox/default/default_night.png");
		defaultSkybox = Skybox(defaultCubemap);

		// Set defaultr skybox as active
		selectedSkybox = defaultSkybox;
	}

	inspectorCamera.transform.position.y = 2.0f;
	inspectorCamera.transform.rotation.x = 22.0f;

}

void Runtime::setupScripts() {

	// Create forward pass
	forwardPass.create(msaaSamples);

	// Create pre pass
	prePass.create();

	// Setup ambient occlusion pass
	ssaoPass.create();

	// Setup post processing
	postProcessingPipeline.create();

	// Setup engine ui
	EditorUI::setup();

	// Setup input system
	Input::setupInputs();

	// Create primitives
	Quad::create();

	// Setup quick gizmo
	QuickGizmo::setup();

}

void Runtime::prepareFrameInternal() {

	// Update times
	time = static_cast<float>(glfwGetTime());
	deltaTime = time - lastTime;
	lastTime = time;
	fps = 1.0f / deltaTime;

	averageFpsElapsedTime += deltaTime;
	averageFpsFrameCount++;
	if (averageFpsElapsedTime >= 1.0f)
	{
		averageFps = static_cast<float>(averageFpsFrameCount) / averageFpsElapsedTime;
		averageFpsElapsedTime = 0.0f;
		averageFpsFrameCount = 0;
	}

	// Reset diagnostics
	currentDrawCalls = 0;
	currentVertices = 0;
	currentPolygons = 0;

}

void Runtime::prepareFrameExternal() {

	// Start new frame for quick gizmos
	QuickGizmo::newFrame();

	// Update input system
	Input::updateInputs();

}

void Runtime::renderFrame() {

	Profiler::start("render");

	// Select camera to be rendered (depending on whether inspector mode is activated)
	if (!inspectorMode)
	{
		renderCamera = activeCamera;
	}
	else
	{
		renderCamera = inspectorCamera;
		InspectorMode::refreshInspector();
	}

	// Get transformation matrices
	glm::mat4 viewMatrix = Transformation::viewMatrix(renderCamera);
	glm::mat4 projectionMatrix = Transformation::projectionMatrix(renderCamera, sceneViewport);
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix)));

	// Set mesh renderers transformation matrix caches for upcomming render passes
	MeshRenderer::currentViewMatrix = viewMatrix;
	MeshRenderer::currentProjectionMatrix = projectionMatrix;
	MeshRenderer::currentViewProjectionMatrix = viewProjectionMatrix;
	MeshRenderer::currentViewNormalMatrix = viewNormalMatrix;

	// Update cameras frustum
	renderCamera.updateFrustum(viewProjectionMatrix);

	// Reset entity metrics
	nCPUEntities = 0;
	nGPUEntities = 0;

	//
	// PREPARATION PASS
	// Prepare each mesh for upcoming render passes
	//
	for (int i = 0; i < entityStack.size(); i++)
	{
		// Could be moved to existing iteration over entity links within some pass to avoid additional iteration overhead
		// Here for now to ensure preparation despite further pipeline changes
		entityStack[i]->meshRenderer.prepareNextFrame();
	}

	//
	// SHADOW PASS
	// Render shadow map
	//
	Profiler::start("shadow_pass");
	mainShadowMap->render();
	Profiler::stop("shadow_pass");

	//
	// PRE PASS
	// Create geometry pass with depth buffer before forward pass
	//
	Profiler::start("pre_pass");
	prePass.render();
	Profiler::stop("pre_pass");
	prePassDepthOutput = prePass.getDepthOutput();
	prePassNormalOutput = prePass.getNormalOutput();

	//
	// SCREEN SPACE AMBIENT OCCLUSION PASS
	// Calculate screen space ambient occlusion if enabled
	//
	unsigned int ssaoOutput = 0;
	if (PostProcessing::configuration.ambientOcclusion)
	{
		ssaoOutput = ssaoPass.render(prePassDepthOutput, prePassNormalOutput);
	}
	ssaoBuffer = ssaoOutput;

	//
	// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
	// Includes injected pre pass
	//
	Profiler::start("forward_pass");
	unsigned int forwardPassOutput = forwardPass.render();
	Profiler::stop("forward_pass");

	//
	// POST PROCESSING PASS
	// Render post processing pass to screen using forward pass output as input
	//
	Profiler::start("post_processing");
	postProcessingPipeline.render(forwardPassOutput);
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

void Runtime::checkResize() {

	if (!resized) return;

	// Destroy all passes/pipelines which are bound to a fixed viewport size
	forwardPass.destroy();
	prePass.destroy();
	ssaoPass.destroy();
	postProcessingPipeline.destroy();

	// Recreate all destroyed passes/pipelines
	forwardPass.create(msaaSamples);
	prePass.create();
	ssaoPass.create(); // Issues with ssao resizing
	postProcessingPipeline.create();

	Log::printProcessDone("Context", "Resize operation performed, various viewport dependant passes recreated");

	resized = false;

}

void Runtime::startupWindow() {



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