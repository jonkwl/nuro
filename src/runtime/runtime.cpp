#include "runtime.h"

// Default runtime values

std::vector<Entity*> Runtime::entityLinks;

UnlitMaterial* Runtime::defaultMaterial = nullptr;
Cubemap* Runtime::defaultSky = nullptr;
Skybox* Runtime::defaultSkybox = nullptr;

Shader* Runtime::prePassShader = nullptr;
Shader* Runtime::shadowPassShader = nullptr;

Camera* Runtime::renderCamera = new Camera();
Camera* Runtime::activeCamera = new Camera();
Camera* Runtime::inspectorCamera = new Camera();

float Runtime::time = 0.0f;
float Runtime::lastTime = 0.0f;
float Runtime::deltaTime = 0.0f;
int Runtime::fps = 0;
float Runtime::averageFps = 0.0f;

glm::vec4 Runtime::clearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
unsigned int Runtime::msaaSamples = 8;
bool Runtime::vsync = false;
bool Runtime::wireframe = false;
bool Runtime::solidMode = false;
bool Runtime::shadows = true;
bool Runtime::postProcessingEffects = true;

bool Runtime::inspectorMode = true;
bool Runtime::showEngineUI = false;
bool Runtime::showDiagnostics = true;

Skybox* Runtime::activeSkybox = nullptr;

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

int Runtime::averageFpsFrameCount = 0;
float Runtime::averageFpsElapsedTime = 0.0f;

bool skipSkyboxLoad = false; // tmp

void Runtime::linkEntity(Entity* entity)
{
	entityLinks.push_back(entity);
}

void Runtime::useCamera(Camera* camera) {
	activeCamera = camera;
}

Camera* Runtime::getCameraRendering()
{
	return renderCamera;
}

Camera* Runtime::getActiveCamera()
{
	return activeCamera;
}

Camera* Runtime::getInspectorCamera()
{
	return inspectorCamera;
}

void glfw_error_callback(int error, const char* description)
{
	Log::printError("GLFW", "Error: " + std::to_string(error), description);
}

void saveDepthMapAsImage(int width, int height, const std::string& filename) {
	std::vector<float> depthData(width * height);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, &depthData[0]);
	std::vector<unsigned char> depthImage(width * height);
	for (int i = 0; i < width * height; ++i) {
		depthImage[i] = static_cast<unsigned char>(depthData[i] * 255.0f);
	}
	std::vector<unsigned char> flippedImage(width * height);
	for (int y = 0; y < height; ++y) {
		memcpy(&flippedImage[y * width], &depthImage[(height - 1 - y) * width], width);
	}
	if (stbi_write_png(filename.c_str(), width, height, 1, &flippedImage[0], width) != 0) {
		Log::printProcessDone("Depth Map", "Depth map saved as " + filename);
	}
	else {
		Log::printError("Depth Map", "Failed to save depth map at " + filename);
	}
}

int Runtime::START_LOOP() {
	//
	// SETUP PHASE 1: CREATE CONTEXT AND LOAD GRAPHICS API //
	//

	Log::printProcessStart("Runtime", "Creating context...");

	// Set error callback and initialize context
	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Check for fullscreen
	if (Window::fullscreen) {
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		Window::width = mode->width;
		Window::height = mode->height;
	}

	// Create window
	Window::glfw = glfwCreateWindow(Window::width, Window::height, Window::title.c_str(), nullptr, nullptr);
	glfwSetFramebufferSizeCallback(Window::glfw, Window::framebuffer_size_callback);
	if (Window::glfw == nullptr) {
		Log::printError("GLFW", "Creation of window failed");
	}

	// Load graphics api
	glfwMakeContextCurrent(Window::glfw);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Log::printError("GLFW", "Initialization of GLAD failed");
	}

	// Debug graphics api version
	const char* version = (const char*)glGetString(GL_VERSION);
	Log::printProcessDone("GLFW", "Initialized, OpenGL version: " + std::string(version));

	// Setup render settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(vsync ? 1 : 0); // V-Sync

	Log::printProcessDone("Runtime", "Context created");

	//
	// SETUP PHASE 2: LOAD ASSETS, COMPILE SHADERS
	//

	// Loading all shaders
	std::vector<std::string> shader_paths = { 
		"./resources/shaders/materials", 
		"./resources/shaders/postprocessing",
		"./resources/shaders/passes" };
	ShaderPool::loadAndCompile(shader_paths);

	prePassShader = ShaderPool::get("pre_pass");
	shadowPassShader = ShaderPool::get("shadow_pass");

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
	if (!skipSkyboxLoad) {

		// Load default sky cubemap
		defaultSky = Cubemap::GetBySingle("./resources/skybox/default/default_night.png");
		/*defaultSky = Cubemap::GetByFaces(
			"./resources/skybox/environment/right.jpg",
			"./resources/skybox/environment/left.jpg",
			"./resources/skybox/environment/top.jpg",
			"./resources/skybox/environment/bottom.jpg",
			"./resources/skybox/environment/front.jpg",
			"./resources/skybox/environment/back.jpg"
		);*/

		// Create default skybox
		defaultSkybox = new Skybox(defaultSky);
		
		// Set defaultr skybox as active
		activeSkybox = defaultSkybox;

	}

	// Set inspector camera data
	inspectorCamera->transform.position.y = 2.0f;
	inspectorCamera->transform.rotation.x = 22.0f;

	//
	// SETUP PHASE 3: CALL ANY OTHER SCRIPTS NEEDING SETUP
	//

	// Set defualt window cursor
	Window::setCursor(Window::cursorMode);

	// Setup forward pass framebuffer
	ForwardPass::setup(msaaSamples);

	// Setup pre pass
	PrePass::setup(Window::width, Window::height);

	// Setup post processing
	PostProcessing::setup();

	// Setup engine ui
	EngineUI::setup();

	// Setup input system
	Input::setupInputs();

	// Create primitives
	Quad::create(); 

	// Setup quick gizmo
	QuickGizmo::setup();

	//
	// SETUP PHASE 4 (FINAL): AWAKE GAME LOGIC
	//
	awake();

	while (!glfwWindowShouldClose(Window::glfw)) {

		unsigned int width = Window::width, height = Window::height;

		//
		// PREPARE COMING FRAME UPDATE
		//

		// Update times
		time = glfwGetTime();
		deltaTime = time - lastTime;
		lastTime = time;
		fps = static_cast<int>(1.0f / deltaTime);

		averageFpsElapsedTime += deltaTime;
		averageFpsFrameCount++;
		if (averageFpsElapsedTime >= 1.0f) {
			averageFps = static_cast<float>(averageFpsFrameCount) / averageFpsElapsedTime;
			averageFpsElapsedTime = 0.0f;
			averageFpsFrameCount = 0;
		}

		// Reset diagnostics
		currentDrawCalls = 0;
		currentVertices = 0;
		currentPolygons = 0;

		//
		// UPDATE ANY SCRIPTS NEEDING UPDATE
		//

		// Update input system
		Input::updateInputs();

		// Start new frame for quick gizmos
		QuickGizmo::newFrame();

		//
		// EXTERNAL TRANSFORM MANIPULATION (e.g. physics)
		// (NONE)
		// 

		//
		// UPDATE GAME LOGIC
		//
		update();

		//
		// RENDER NEXT FRAME (full render pipeline pass)
		//

		Profiler::start("render");

		// Select camera to be rendered (depending on whether inspector mode is activated)
		if (!inspectorMode) {
			renderCamera = activeCamera;
		}
		else {
			renderCamera = inspectorCamera;
			InspectorMode::refreshInspector();
		}

		// Get view and projection matrices
		glm::mat4 viewMatrix = Transformation::viewMatrix(renderCamera);
		glm::mat4 projectionMatrix = Transformation::projectionMatrix(renderCamera, width, height);

		// Set mesh renderers view and projection matrix for upcomming render passes
		MeshRenderer::currentViewMatrix = viewMatrix;
		MeshRenderer::currentProjectionMatrix = projectionMatrix;

		// Update cameras frustum
		renderCamera->updateFrustum(viewMatrix, projectionMatrix);

		nCPUEntities = 0;
		nGPUEntities = 0;

		// Enable culling
		glEnable(GL_CULL_FACE);

		//
		// SHADOW PASS
		// Render shadow map
		//
		Profiler::start("shadow_pass");
		mainShadowMap->render();
		Profiler::stop("shadow_pass");

		// Save shadow map (tmp)
		/* if (!shadow_map_saved) {
			glBindFramebuffer(GL_FRAMEBUFFER, mainShadowMap->getFramebuffer());
			saveDepthMapAsImage(mainShadowMap->getWidth(), mainShadowMap->getHeight(), "./shadow_map.png");
			shadow_map_saved = true;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		} */

		//
		// PRE PASS
		// Create geometry pass with depth buffer before forward pass
		//
		// Not needed in current pipeline
		/*Profiler::start("pre_pass");
		PrePass::render();
		Profiler::stop("pre_pass");*/

		//
		// FORWARD PASS: Perform rendering for every object with materials, lighting etc.
		//
		Profiler::start("forward_pass");
		unsigned int FORWARD_PASS_OUTPUT = ForwardPass::render();
		Profiler::stop("forward_pass");

		//
		// POST PROCESSING PASS
		// Render post processing pass to screen using forward pass output as input
		//
		Profiler::start("post_processing");
		PostProcessing::render(FORWARD_PASS_OUTPUT);
		Profiler::stop("post_processing");

		//
		// ENGINE UI PASS
		// Render engine ui to screen
		//
		Profiler::start("ui_pass");
		EngineUI::newFrame();
		EngineUI::render();
		Profiler::stop("ui_pass");

		//
		// MANAGE FRAME BUFFER AND WINDOW CONTEXT AND PROCESS EVENTS
		//
		glfwSwapBuffers(Window::glfw);
		glfwPollEvents();

		Profiler::stop("render");
	}

	// Exit application

	glfwTerminate();
	return 0;
}

void Runtime::TERMINATE()
{
	if (Window::glfw != nullptr) {
		glfwDestroyWindow(Window::glfw);
		glfwTerminate();
	}
	std::exit(0);
}