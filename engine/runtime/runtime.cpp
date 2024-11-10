#include "runtime.h"

// Default runtime values

std::vector<EntityProcessor*> Runtime::entityLinks;

UnlitMaterial* Runtime::defaultMaterial = nullptr;
Cubemap* Runtime::defaultSky = nullptr;
Skybox* Runtime::defaultSkybox = nullptr;

Shader* Runtime::depthPrePassShader = nullptr;
Shader* Runtime::shadowPassShader = nullptr;

Camera* Runtime::renderCamera = new Camera();
Camera* Runtime::activeCamera = new Camera();
Camera* Runtime::inspectorCamera = new Camera();

float Runtime::time = 0.0f;
float Runtime::lastTime = 0.0f;
float Runtime::deltaTime = 0.0f;
int Runtime::fps = 0;
float Runtime::averageFps = 0.0f;

glm::vec4 Runtime::clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
bool Runtime::vsync = false;
bool Runtime::wireframe = false;
bool Runtime::solidMode = false;
bool Runtime::shadows = true;
bool Runtime::postProcessingEffects = true;

bool Runtime::inspectorMode = true;
bool Runtime::showEngineUI = false;
bool Runtime::showDiagnostics = true;

Skybox* Runtime::activeSkybox = nullptr;

DepthPrePass* Runtime::depthPrePass = nullptr;

ShadowMap* Runtime::mainShadowMap = nullptr;

unsigned int Runtime::currentDrawCalls = 0;
unsigned int Runtime::currentVertices = 0;
unsigned int Runtime::currentPolygons = 0;
double Runtime::renderDuration = 0.0;
double Runtime::shadowPassDuration = 0.0;
double Runtime::forwardPassDuration = 0.0;
double Runtime::uiPassDuration = 0.0;

float Runtime::directionalIntensity = 0.2f;
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

bool settingA = false;
bool settingB = false;
bool settingC = false;
bool settingD = false;

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
		"./resources/shaders/prepass",
		"./resources/shaders/shadows" };
	ShaderBuilder::loadAndCompile(shader_paths);

	depthPrePassShader = ShaderBuilder::get("depth_pre_pass");
	shadowPassShader = ShaderBuilder::get("shadow_pass");

	// Creating default material
	defaultMaterial = new UnlitMaterial();
	defaultMaterial->baseColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	// Create default shadow map
	bool shadow_map_saved = false;
	mainShadowMap = new ShadowMap(4096);

	// Creating default skybox
	defaultSky = Cubemap::GetBySingle("./resources/skybox/default/default_mini.png");
	/*defaultSky = Cubemap::GetByFaces(
		"./resources/skybox/environment/right.jpg",
		"./resources/skybox/environment/left.jpg",
		"./resources/skybox/environment/top.jpg",
		"./resources/skybox/environment/bottom.jpg",
		"./resources/skybox/environment/front.jpg",
		"./resources/skybox/environment/back.jpg"
	);*/
	defaultSkybox = new Skybox(defaultSky);

	// Setup post processing
	PostProcessing::initialize();

	// Set inspector camera data
	inspectorCamera->transform.position.y = 2.0f;
	inspectorCamera->transform.rotation.x = 22.0f;

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

	while (!glfwWindowShouldClose(Window::glfw)) {
		auto renderStart = std::chrono::high_resolution_clock::now();

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
		glm::mat4 view = Transformation::viewMatrix(renderCamera);
		glm::mat4 projection = Transformation::projectionMatrix(renderCamera, width, height);
		EntityProcessor::currentViewMatrix = view;
		EntityProcessor::currentProjectionMatrix = projection;

		//
		// SHADOW PASS: Render shadow map
		//

		auto shadowPassStart = std::chrono::high_resolution_clock::now();

		mainShadowMap->render();

		auto shadowPassEnd = std::chrono::high_resolution_clock::now();
		shadowPassDuration = std::chrono::duration<double, std::milli>(shadowPassEnd - shadowPassStart).count();

		// Save depth map
		if (!shadow_map_saved) {
			glBindFramebuffer(GL_FRAMEBUFFER, mainShadowMap->getFramebuffer());
			saveDepthMapAsImage(mainShadowMap->getSize(), mainShadowMap->getSize(), "./shadow_map.png");
			shadow_map_saved = true;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		//
		// FORWARD RENDERING PASS: Render next frame
		//

		auto forwardPassStart = std::chrono::high_resolution_clock::now();
		
		// Set viewport and bind post processing framebuffer
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width, height);
		PostProcessing::bind();

		// Render wireframe if enabled
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Render each linked entity
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < entityLinks.size(); i++) {
			entityLinks.at(i)->forwardPass();
		}

		// Disable wireframe if enabled
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Render skybox
		if (activeSkybox != nullptr) {
			activeSkybox->render(view, projection);
		}

		// Render framebuffer
		PostProcessing::render();

		auto forwardPassEnd = std::chrono::high_resolution_clock::now();
		forwardPassDuration = std::chrono::duration<double, std::milli>(forwardPassEnd - forwardPassStart).count();

		//
		// UPDATE PHASE 7: RENDER ENGINE UI
		//

		auto uiPassStart = std::chrono::high_resolution_clock::now();

		EngineUI::newFrame();

		// Inspector mode ui
		if (inspectorMode) {
			if (showEngineUI) {
				EngineDialog::vec3_dialog("Directional Light: Direction", directionalDirection);

				EngineDialog::bool_dialog("Wireframe", wireframe);

				InputPair x = {"Directional Intensity", directionalIntensity, 0.0f, 5.0f};
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
				EngineDialog::input_dialog("Basic Settings @ Post Processing", { x, a, b, c, d, e, f, g, h, i, j, k });

				EngineDialog::bool_dialog("Chromatic Aberration", PostProcessing::setup.chromaticAberration);
				EngineDialog::bool_dialog("Vignette", PostProcessing::setup.vignette);

				EngineDialog::float_dialog("PL1 Intensity", intensity, 0.0f, 12.0f);
				EngineDialog::float_dialog("PL1 Range", range, 0.0f, 15.0f);
				EngineDialog::float_dialog("PL1 Falloff", falloff, 0.0f, 30.0f);

				ImGui::Begin("Normal Mapping", nullptr, EngineUI::windowFlags.fixed);
					UIComponents::headline("Normal Mapping", ICON_FA_CUBES);

					UILayout::beginFlex("Normal Mapping Settings", ROW, FULL_WIDTH, 45.0f, JUSTIFY_START, ALIGN_CENTER, 5.0f);
						if (ImGui::Button(normalMapping ? "Disable Normal Mapping" : "Enable Normal Mapping")) {
							normalMapping = !normalMapping;
						}
						if (normalMapping) {
							ImGui::SliderFloat("Intensity", &normalMappingIntensity, 0.0f, 10.0f);
						}
					UILayout::endFlex();
				ImGui::End();

				ImGui::Begin("  Post Processing  ", nullptr, EngineUI::windowFlags.standard);
					UIComponents::headline("Post Processing", ICON_FA_SPARKLES, HEADLINE_LEFT);

					if (UIComponents::extendableSettings("Ambient Occlusion", settingA, ICON_FA_PROJECTOR)) {
						ImGui::Text("Ambient Occlusion Option 1");
						ImGui::Text("Ambient Occlusion Option 2");
						ImGui::Text("Ambient Occlusion Option 3");
						ImGui::Text("Ambient Occlusion Option 4");
						ImGui::Text("Ambient Occlusion Option 5");
						UIComponents::space(0.0f, 2.5f);
					}

					if (UIComponents::extendableSettings("Vignette", settingB, ICON_FA_BAG_SHOPPING)) {
						ImGui::Text("Vignette Option 1");
						ImGui::Text("Vignette Option 2");
						ImGui::Text("Vignette Option 3");
						ImGui::Text("Vignette Option 4");
						ImGui::Text("Vignette Option 5");
						UIComponents::space(0.0f, 2.5f);
					}

					if (UIComponents::extendableSettings("Color Correction", settingC, ICON_FA_COMMENT)) {
						ImGui::Text("Color Correction Option 1");
						ImGui::Text("Color Correction Option 2");
						ImGui::Text("Color Correction Option 3");
						ImGui::Text("Color Correction Option 4");
						ImGui::Text("Color Correction Option 5");
						UIComponents::space(0.0f, 2.5f);
					}

					if (UIComponents::extendableSettings("Depth Of Field", settingD, ICON_FA_PRINT)) {
						ImGui::Text("Depth Of Field Option 1");
						ImGui::Text("Depth Of Field Option 2");
						ImGui::Text("Depth Of Field Option 3");
						ImGui::Text("Depth Of Field Option 4");
						ImGui::Text("Depth Of Field Option 5");
						UIComponents::space(0.0f, 2.5f);
					}

				ImGui::End();
			}
		}

		// Diagnostics ui
		if (showDiagnostics) {
			EngineDialog::show_diagnostics(deltaTime, fps, averageFps);
		}

		EngineUI::render();

		auto uiPassEnd = std::chrono::high_resolution_clock::now();
		uiPassDuration = std::chrono::duration<double, std::milli>(uiPassEnd - uiPassStart).count();

		//
		// UPDATE PHASE 8: MANAGE FRAME BUFFER AND WINDOW CONTEXT AND PROCESS EVENTS
		//

		glfwSwapBuffers(Window::glfw);
		glfwPollEvents();

		auto renderEnd = std::chrono::high_resolution_clock::now();
		renderDuration = std::chrono::duration<double, std::milli>(renderEnd - renderStart).count();
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