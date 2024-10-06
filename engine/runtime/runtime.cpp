#include "runtime.h"

#include <stb_image_write.h>

// Default runtime values

std::vector<EntityProcessor*> Runtime::entityLinks;

Texture* Runtime::defaultDiffuseTexture = nullptr;
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

bool Runtime::wireframe = false;

bool Runtime::inspectorMode = true;
bool Runtime::showEngineUI = false;
bool Runtime::showDiagnostics = false;

Skybox* Runtime::activeSkybox = nullptr;

unsigned int Runtime::shadowMap = 0;
unsigned int Runtime::shadowMapSize = 4096;
unsigned int Runtime::shadowMapFramebuffer = 0;

float Runtime::lightIntensity = 0.3f;
glm::vec3 Runtime::lightPosition = glm::vec3(5.0f, 5.0f, -5.0f);

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

void Runtime::bindShadowMap(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}

void glfw_error_callback(int error, const char* description)
{
	Log::printError("GLFW", "Error: " + std::to_string(error), description);
}

void createShadowMap(unsigned int& depthMap, unsigned int size, unsigned int& fbo) {
	glGenFramebuffers(1, &fbo);

	const unsigned int SHADOW_MAP_WIDTH = size, SHADOW_MAP_HEIGHT = size;

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4); // Create buffer for anti aliasing

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

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	}

	// Setup render settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE); // MSAA Anti Aliasing
	// glEnable(GL_FRAMEBUFFER_SRGB); // Default Gamma Correction (Handled by post processing) 
	glfwSwapInterval(1); // V-Sync

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

	// Creating default texture
	defaultDiffuseTexture = new Texture("./resources/textures/default.jpg", DIFFUSE);

	// Creating default material
	defaultMaterial = new UnlitMaterial(defaultDiffuseTexture);
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
	createShadowMap(shadowMap, shadowMapSize, shadowMapFramebuffer);

	while (!glfwWindowShouldClose(Window::glfw)) {
		unsigned int width = Window::width, height = Window::height;

		//
		// UPDATE PHASE 1: UPDATE GLOBAL TIMES
		//

		time = glfwGetTime();
		deltaTime = time - lastTime;
		lastTime = time;
		fps = static_cast<int>(1.0f / deltaTime);

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

		// Set viewport and bind shadow map framebuffer
		glViewport(0, 0, shadowMapSize, shadowMapSize);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Get shadow map transformation matrices
		glm::mat4 lightProjection = Transformation::lightProjectionMatrix(renderCamera);
		glm::mat4 lightView = Transformation::lightViewMatrix(lightPosition);
		glm::mat4 lightSpace = lightProjection * lightView;
		EntityProcessor::currentLightSpace = lightSpace;

		// Bind shadow pass shader and render each objects depth on shadow map
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);
		shadowPassShader->bind();
		for (int i = 0; i < entityLinks.size(); i++) {
			entityLinks.at(i)->shadowPass();
		}
		glCullFace(GL_BACK);

		// Save depth map
		if (!depth_map_saved) {
			saveDepthMapAsImage(shadowMapSize, shadowMapSize, "./depth_map.png");
			depth_map_saved = true;
		}

		// Unbind shadow map framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		  
		//
		// UPDATE PHASE 6: FORWARD RENDERING PASS: Render next frame
		//
		
		// Set viewport and bind post processing framebuffer
		glViewport(0, 0, width, height);
		PostProcessing::bind();

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
				EngineDialog::vec3_dialog("Camera Position", renderCamera->position);
				EngineDialog::vec3_dialog("Camera Rotation", renderCamera->rotation, -360.0f, 360.0f);
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

				if (wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}

		// Diagnostics ui
		if (showDiagnostics) {
			EngineDialog::show_diagnostics(fps);
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