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

PrePass* Runtime::prePass = nullptr;

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

int Runtime::averageFpsFrameCount = 0;
float Runtime::averageFpsElapsedTime = 0.0f;

bool Runtime::normalMapping = true;
float Runtime::normalMappingIntensity = 1.0f;

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

void Runtime::setSkybox(Skybox* skybox)
{
	activeSkybox = skybox;
}

Skybox* Runtime::getActiveSkybox()
{
	return activeSkybox;
}

std::vector<Entity*> Runtime::getEntityLinks()
{
	return entityLinks;
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
		"./resources/shaders/passes",
		"./resources/shaders/shadows" };
	ShaderBuilder::loadAndCompile(shader_paths);

	prePassShader = ShaderBuilder::get("pre_pass");
	shadowPassShader = ShaderBuilder::get("shadow_pass");

	// Creating default material
	defaultMaterial = new UnlitMaterial();
	defaultMaterial->baseColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	// Create shadow disk
	unsigned int diskWindowSize = 4;
	unsigned int diskFilterSize = 8;
	unsigned int diskRadius = 7;
	mainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);

	// Create default shadow map
	bool shadow_map_saved = false;
	mainShadowMap = new ShadowMap(4096, 4096, 40.0f, 40.0f);

	// Creating default skybox
	if (!skipSkyboxLoad) {

		defaultSky = Cubemap::GetBySingle("./resources/skybox/default/default_night.png");
		/*defaultSky = Cubemap::GetByFaces(
			"./resources/skybox/environment/right.jpg",
			"./resources/skybox/environment/left.jpg",
			"./resources/skybox/environment/top.jpg",
			"./resources/skybox/environment/bottom.jpg",
			"./resources/skybox/environment/front.jpg",
			"./resources/skybox/environment/back.jpg"
		);*/
		defaultSkybox = new Skybox(defaultSky);

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
	ForwardPassFrame::setup(msaaSamples);

	// Setup post processing
	PostProcessing::setup();

	// Setup engine ui
	EngineUI::setup();

	// Setup input system
	Input::setupInputs();

	// Create primitives
	Quad::create(); 

	//
	// SETUP PHASE 4 (FINAL): AWAKE GAME LOGIC
	//
	awake();

	while (!glfwWindowShouldClose(Window::glfw)) {

		Profiler::start("render");

		unsigned int width = Window::width, height = Window::height;

		//
		// PREPARE COMING RENDER PASSES
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

		Input::updateInputs();

		//
		// EXTERNAL TRANSFORM MANIPULATION (e.g. physics)
		// (NONE)
		// 

		//
		// UPDATE GAME LOGIC
		//
		update();

		//
		// SELECT CAMERA TO BE RENDERED AND CALCULATE VIEW & PROJECTION MATRICES
		//

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
		renderCamera->frustum.update(viewMatrix * projectionMatrix);

		// Enable culling
		glEnable(GL_CULL_FACE);

		//
		// SHADOW PASS: Render shadow map
		//

		Profiler::start("shadow_pass");

		mainShadowMap->render();

		Profiler::stop("shadow_pass");

		// Save shadow map
		if (!shadow_map_saved) {
			glBindFramebuffer(GL_FRAMEBUFFER, mainShadowMap->getFramebuffer());
			// saveDepthMapAsImage(mainShadowMap->getWidth(), mainShadowMap->getHeight(), "./shadow_map.png");
			shadow_map_saved = true;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		//
		// FORWARD RENDERING PASS: Render next frame
		//

		Profiler::start("forward_pass");

		// Bind forward pass framebuffer
		ForwardPassFrame::bind();

		// Set culling to back face
		glCullFace(GL_BACK);

		// Set viewport and bind post processing framebuffer
		if (!wireframe) {
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		}
		else {
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		// Set wireframe if enabled
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Render each linked entity to bound forward pass frame
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < entityLinks.size(); i++) {
			Entity* entity = entityLinks.at(i);
			entity->meshRenderer->currentModelMatrix = Transformation::modelMatrix(entity);
			entity->meshRenderer->forwardPass();
		}

		// Disable wireframe if enabled
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Disable culling before rendering skybox
		glDisable(GL_CULL_FACE);

		// Render skybox to bound forward pass frame
		if (activeSkybox != nullptr && !wireframe) {
			activeSkybox->render(viewMatrix, projectionMatrix);
		}

		// Bilt forward pass framebuffer
		unsigned int FORWARD_PASS_OUTPUT = ForwardPassFrame::blit();

		Profiler::stop("forward_pass");

		//
		// POST PROCESSING PASS
		//

		Profiler::start("post_processing");

		// Render post processing pass to screen using forward pass output as input
		PostProcessing::render(FORWARD_PASS_OUTPUT);

		Profiler::stop("post_processing");

		//
		// RENDER ENGINE UI
		//

		Profiler::start("ui_pass");

		EngineUI::newFrame();

		EngineUI::render();

		Profiler::stop("ui_pass");

		//
		// UPDATE PHASE 8: MANAGE FRAME BUFFER AND WINDOW CONTEXT AND PROCESS EVENTS
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