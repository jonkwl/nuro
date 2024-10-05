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

void glfw_error_callback(int error, const char* description)
{
	Log::printError("GLFW", "Error: " + std::to_string(error), description);
}

void getShadowMap(unsigned int size, unsigned int& fbo, unsigned int& depthMap) {
	glGenFramebuffers(1, &fbo);

	const unsigned int SHADOW_MAP_WIDTH = size, SHADOW_MAP_HEIGHT = size;

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void saveDepthMapAsImage(int width, int height, const std::string& filename) {
	// Create a buffer to hold the depth data
	std::vector<float> depthData(width * height);

	// Read the depth data from the framebuffer
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, &depthData[0]);

	// Normalize the depth data to the range [0, 255] for visualization
	std::vector<unsigned char> depthImage(width * height);
	for (int i = 0; i < width * height; ++i) {
		// Depth values are typically in the range [0, 1], map them to [0, 255]
		depthImage[i] = static_cast<unsigned char>(depthData[i] * 255.0f);
	}

	// Flip the image vertically because OpenGL's origin is bottom-left
	std::vector<unsigned char> flippedImage(width * height);
	for (int y = 0; y < height; ++y) {
		memcpy(&flippedImage[y * width], &depthImage[(height - 1 - y) * width], width);
	}

	// Save the image using stb_image_write
	if (stbi_write_png(filename.c_str(), width, height, 1, &flippedImage[0], width) != 0) {
		std::cout << "Depth map saved as " << filename << std::endl;
	}
	else {
		std::cerr << "Failed to save the depth map." << std::endl;
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

	if (Runtime::wireframe) {
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

	// Creating default texture
	Runtime::defaultDiffuseTexture = new Texture("./resources/textures/default.jpg", DIFFUSE);

	// Creating default material
	Runtime::defaultMaterial = new UnlitMaterial(Runtime::defaultDiffuseTexture);
	Runtime::defaultMaterial->baseColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	// Creating default skybox
	// Runtime::defaultSky = Cubemap::GetBySingle("./resources/skybox/default/default.jpg");
	Runtime::defaultSky = Cubemap::GetByFaces(
		"./resources/skybox/environment/right.jpg",
		"./resources/skybox/environment/left.jpg",
		"./resources/skybox/environment/top.jpg",
		"./resources/skybox/environment/bottom.jpg",
		"./resources/skybox/environment/front.jpg",
		"./resources/skybox/environment/back.jpg"
	);
	Runtime::defaultSkybox = new Skybox(Runtime::defaultSky);

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
	unsigned int shadow_map_fbo;
	unsigned int shadow_map_texture;
	unsigned int shadow_map_size = 1024;
	getShadowMap(shadow_map_size, shadow_map_fbo, shadow_map_texture);
	Shader* shadow_pass_shader = ShaderBuilder::get("shadow_pass");

	while (!glfwWindowShouldClose(Window::glfw)) {
		unsigned int width = Window::width, height = Window::height;

		//
		// UPDATE PHASE 1: UPDATE GLOBAL TIMES
		//

		Runtime::time = glfwGetTime();
		Runtime::deltaTime = Runtime::time - Runtime::lastTime;
		Runtime::lastTime = Runtime::time;
		Runtime::fps = static_cast<int>(1.0f / Runtime::deltaTime);

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

		// Set viewport, bind shadow map framebuffer and clear buffers
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, shadow_map_size, shadow_map_size);
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Create shadow map transformation matrices
		glm::mat4 light_projection = Transformation::lightProjectionMatrix(renderCamera);
		glm::mat4 light_view = Transformation::lightViewMatrix(Runtime::lightPosition);
		glm::mat4 light_space = light_projection * light_view;

		// Bind shadow pass shader render every mesh of each object
		shadow_pass_shader->bind();
		for (int i = 0; i < entityLinks.size(); i++) {
			Entity* entity = entityLinks.at(i)->entity;
			if (entity->model == nullptr) continue;
			Model* model = entity->model;
			for (int a = 0; a < model->meshes.size(); a++) {
				Mesh* mesh = model->meshes.at(a);
				mesh->bind();
				glm::mat4 modelMatrix = Transformation::modelMatrix(entity);
				shadow_pass_shader->setMatrix4("lightSpace", light_space);
				shadow_pass_shader->setMatrix4("model", modelMatrix);
				mesh->render();
			}
		}

		// Save depth map
		if (!depth_map_saved) {
			saveDepthMapAsImage(shadow_map_size, shadow_map_size, "./depth_map.png");
			depth_map_saved = true;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		  
		//
		// UPDATE PHASE 6: FORWARD RENDERING PASS: Render next frame
		//
		
		// Set viewport, bind post processing framebuffer and clear buffers
		glViewport(0, 0, width, height);
		PostProcessing::bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Select camera to be rendered (depending on whether inspector mode is activated)
		if (!Runtime::inspectorMode) {
			Runtime::renderCamera = Runtime::activeCamera;
		}
		else {
			Runtime::renderCamera = Runtime::inspectorCamera;
			InspectorMode::refreshInspector();
		}

		// Get view and projection matrices
		glm::mat4 view = Transformation::viewMatrix(renderCamera);
		glm::mat4 projection = Transformation::projectionMatrix(renderCamera, width, height);

		// Render each linked entity
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < Runtime::entityLinks.size(); i++) {
			Runtime::entityLinks.at(i)->render(view, projection);
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
		if (Runtime::inspectorMode) {
			if (Runtime::showEngineUI) {
				EngineDialog::vec3_dialog("Camera Position", Runtime::renderCamera->position);
				EngineDialog::vec3_dialog("Camera Rotation", Runtime::renderCamera->rotation, -360.0f, 360.0f);
				EngineDialog::float_dialog("FOV", Runtime::renderCamera->fov, 30, 90);
				EngineDialog::bool_dialog("Wireframe", Runtime::wireframe);

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

				if (Runtime::wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}

		// Diagnostics ui
		if (Runtime::showDiagnostics) {
			EngineDialog::show_diagnostics(Runtime::fps);
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