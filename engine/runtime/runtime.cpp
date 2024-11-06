#include "runtime.h"

#include <stb_image_write.h>

// Default runtime values

std::vector<EntityProcessor*> Runtime::entityLinks;

UnlitMaterial* Runtime::defaultMaterial = nullptr;
Cubemap* Runtime::defaultSky = nullptr;
Skybox* Runtime::defaultSkybox = nullptr;

Camera* Runtime::renderCamera = new Camera();
Camera* Runtime::activeCamera = new Camera();
Camera* Runtime::inspectorCamera = new Camera();

float Runtime::time = 0.0f;
float Runtime::lastTime = 0.0f;
float Runtime::deltaTime = 0.0f;
int Runtime::fps = 0;
float Runtime::averageFps = 0.0f;

bool Runtime::vsync = true;
bool Runtime::wireframe = false;

bool Runtime::inspectorMode = true;
bool Runtime::showEngineUI = false;
bool Runtime::showDiagnostics = true;

Skybox* Runtime::activeSkybox = nullptr;

ShadowMap* Runtime::mainShadowMap = nullptr;

float Runtime::lightIntensity = 0.3f;
glm::vec3 Runtime::lightPosition = glm::vec3(3.0f, 5.0f, -5.0f);

float Runtime::roughness = 0.45f;
float Runtime::metallic = 0.2f;

int Runtime::averageFpsFrameCount = 0;
float Runtime::averageFpsElapsedTime = 0.0f;

void Runtime::linkEntity(Entity* entity)
{
	EntityProcessor* entityLink = new EntityProcessor(entity);
	entityLinks.push_back(entityLink);
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

std::vector<EntityProcessor*> Runtime::getEntityLinks()
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
		"./resources/shaders/shadows" };
	ShaderBuilder::loadAndCompile(shader_paths);

	Shader* shadowPassShader = ShaderBuilder::get("shadow_pass");

	// Creating default material
	defaultMaterial = new UnlitMaterial();
	defaultMaterial->baseColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	// Creating default skybox
	// defaultSky = Cubemap::GetBySingle("./resources/skybox/default/default.jpg");
	defaultSky = Cubemap::GetByFaces(
		"./resources/skybox/environment/right.jpg",
		"./resources/skybox/environment/left.jpg",
		"./resources/skybox/environment/top.jpg",
		"./resources/skybox/environment/bottom.jpg",
		"./resources/skybox/environment/front.jpg",
		"./resources/skybox/environment/back.jpg"
	);
	defaultSkybox = new Skybox(defaultSky);

	// Initialize entity processor default fields
	EntityProcessor::linkDefaults(defaultMaterial, shadowPassShader);

	// Setup post processing
	PostProcessing::initialize();

	//
	// SETUP PHASE 3: CALL ANY OTHER SCRIPTS NEEDING SETUP
	//

	Window::setCursor(Window::cursorMode);

	EngineUI::setup();
	Input::setupInputs();

	//
	// SETUP PHASE 4 (FINAL): AWAKE GAME LOGIC
	//
	awake();

	// Create shadow map
	bool depth_map_saved = false;
	mainShadowMap = new ShadowMap(2048, shadowPassShader);

	while (!glfwWindowShouldClose(Window::glfw)) {
		unsigned int width = Window::width, height = Window::height;

		//
		// UPDATE PHASE 1: UPDATE GLOBAL TIMES
		//

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

		//
		// UPDATE PHASE 2: UPDATE ANY SCRIPTS NEEDING UPDATE
		//

		Input::updateInputs();

		//
		// UPDATE PHASE 3: EXTERNAL TRANSFORM MANIPULATION (e.g. physics)
		// (NONE)
		//

		//
		// UPDATE PHASE 4: UPDATE GAME LOGIC
		//
		update();

		//
		// UPDATE PHASE 5: SHADOW PASS: Render shadow map
		//

		mainShadowMap->render(lightPosition);

		// Save depth map
		if (!depth_map_saved) {
			glBindFramebuffer(GL_FRAMEBUFFER, mainShadowMap->getFramebuffer());
			saveDepthMapAsImage(mainShadowMap->getSize(), mainShadowMap->getSize(), "./depth_map.png");
			depth_map_saved = true;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		  
		//
		// UPDATE PHASE 6: FORWARD RENDERING PASS: Render next frame
		//
		
		// Set viewport and bind post processing framebuffer
		glViewport(0, 0, width, height);
		PostProcessing::bind();

		// Render wireframe if enabled
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Select camera to be rendered (depending on whether inspector mode is activated)
		if (!inspectorMode) {
			renderCamera = activeCamera;
		}
		else {
			renderCamera = inspectorCamera;
			InspectorMode::refreshInspector();
		}

		// Get view and projection matrices
		glm::mat4 view = Transformation::viewMatrix(renderCamera);
		glm::mat4 projection = Transformation::projectionMatrix(renderCamera, width, height);
		EntityProcessor::currentView = view;
		EntityProcessor::currentProjection = projection;

		// Render each linked entity
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < entityLinks.size(); i++) {
			entityLinks.at(i)->render();
		}

		// Disable wireframe if enabled
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Render skybox
		if (activeSkybox != nullptr) {
			activeSkybox->render(view, projection);
		}

		// Render framebuffer
		PostProcessing::render();
		
		//
		// UPDATE PHASE 7: RENDER ENGINE UI
		//

		EngineUI::newFrame();

		// Inspector mode ui
		if (inspectorMode) {
			if (showEngineUI) {
				EngineDialog::vec3_dialog("Camera Position", renderCamera->transform.position);
				EngineDialog::vec3_dialog("Camera Rotation", renderCamera->transform.rotation, -360.0f, 360.0f);
				EngineDialog::float_dialog("FOV", renderCamera->fov, 30, 90);
				EngineDialog::bool_dialog("Wireframe", wireframe);

				InputPair x = {"Light Intensity", lightIntensity, 0.0f, 5.0f};
				InputPair a = { "Exposure", PostProcessing::setup.exposure, 0.0f, 10.0f };
				InputPair b = { "Contrast", PostProcessing::setup.contrast, 0.95f, 1.1f };
				InputPair c = { "Gamma", PostProcessing::setup.gamma, 0.0f, 5.0f };
				InputPair d = { "Chromatic Aberration Strength", PostProcessing::setup.chromaticAberrationStrength, 0.0f, 5.0f };
				InputPair e = { "Chromatic Aberration Range", PostProcessing::setup.chromaticAberrationRange, 0.0f, 1.0f };
				InputPair f = { "Chromatic Aberration Red Offset", PostProcessing::setup.chromaticAberrationRedOffset, -0.1f, 0.1f };
				InputPair g = { "Chromatic Aberration Blue Offset", PostProcessing::setup.chromaticAberrationBlueOffset, -0.1f, 0.1f };
				InputPair h = { "Vignette Strength", PostProcessing::setup.vignetteStrength, 0.0f, 1.0f };
				InputPair i = { "Vignette Radius", PostProcessing::setup.vignetteRadius, 0.0f, 1.0f };
				InputPair j = { "Vignette Softness", PostProcessing::setup.vignetteSoftness, 0.0f, 1.0f };
				InputPair k = { "Vignette Roundness", PostProcessing::setup.vignetteRoundness, 0.0f, 2.0f };
				EngineDialog::input_dialog("Post Processing", { x, a, b, c, d, e, f, g, h, i, j, k });

				EngineDialog::bool_dialog("Chromatic Aberration", PostProcessing::setup.chromaticAberration);
				EngineDialog::bool_dialog("Vignette", PostProcessing::setup.vignette);

				EngineDialog::float_dialog("roughness", roughness, 0.0f, 1.0f);
				EngineDialog::float_dialog("metallic", metallic, 0.0f, 1.0f);
			}
		}

		// Diagnostics ui
		if (showDiagnostics) {
			EngineDialog::show_diagnostics(deltaTime, fps, averageFps);
		}

		EngineUI::render();

		//
		// UPDATE PHASE 8: MANAGE FRAME BUFFER AND WINDOW CONTEXT AND PROCESS EVENTS
		//

		glfwSwapBuffers(Window::glfw);
		glfwPollEvents();
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