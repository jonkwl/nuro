#include "runtime.h"

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

Entity* Runtime::createEntity() {
	Entity* entity = new Entity();
	EntityProcessor* entityLink = new EntityProcessor(entity);
	entityLinks.push_back(entityLink);
	return entity;
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

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int throw_err(std::string message) {
	std::cout << "ERROR: " << message << std::endl;
	glfwTerminate();
	return -1;
}

void generate_post_processing_buffers(unsigned int& _fbo, unsigned int& _texture, unsigned int &_vao) {
	float vertices[] =
	{
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Frame buffer object
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Framebuffer texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// Render buffer object
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check for framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	_fbo = fbo;
	_texture = texture;
	_vao = vao;
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
		return throw_err("Creation of window failed");
	}

	// Load graphics api
	glfwMakeContextCurrent(Window::glfw);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return throw_err("Initialization of GLAD failed");
	}

	if (Runtime::wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	}

	// Update context
	Window::setViewport();

	// Setup render settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE); // MSAA Anti Aliasing
	glEnable(GL_FRAMEBUFFER_SRGB); // Default Gamma Correction
	glfwSwapInterval(1); // V-Sync

	Log::printProcessDone("Runtime", "Context created");

	//
	// SETUP PHASE 2: LOAD ASSETS, COMPILE SHADERS
	//

	// Loading all shaders
	std::vector<std::string> shader_paths = { "./resources/shaders/materials", "./resources/shaders/post_processing"};
	ShaderBuilder::loadAndCompile(shader_paths);

	// Creating default texture
	Runtime::defaultDiffuseTexture = new Texture("./resources/textures/default.jpg", DIFFUSE);

	// Creating default material
	Runtime::defaultMaterial = new UnlitMaterial(Runtime::defaultDiffuseTexture);
	Runtime::defaultMaterial->baseColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	// Creating default skybox
	Runtime::defaultSky = Cubemap::GetBySingle("./resources/skybox/default/default.jpg");
	/*Runtime::defaultSky = Cubemap::GetByFaces(
		"./resources/skybox/environment/right.jpg",
		"./resources/skybox/environment/left.jpg",
		"./resources/skybox/environment/top.jpg",
		"./resources/skybox/environment/bottom.jpg",
		"./resources/skybox/environment/front.jpg",
		"./resources/skybox/environment/back.jpg"
	);*/
	Runtime::defaultSkybox = new Skybox(Runtime::defaultSky);

	// Post processing setup
	unsigned int pp_fbo, pp_texture, pp_vao;
	generate_post_processing_buffers(pp_fbo, pp_texture, pp_vao);
	Shader* pp_shader = ShaderBuilder::get("framebuffer");

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

	float contrast = 1.05f;

	bool chromaticAberration = true;
	float chromaticAberrationStrength = 1.15f;
	float chromaticAberrationRange = 0.2f;
	float chromaticAberrationRedOffset = 0.01f;
	float chromaticAberrationBlueOffset = 0.01f;

	bool vignette = true;
	glm::vec4 vignetteColor = glm::vec4(0.0f, 0.0f, 0.0, 0.7f);
	float vignetteRadius = 0.66f;
	float vignetteSoftness = 0.36f;
	float vignetteRoundness = 1.35f;

	while (!glfwWindowShouldClose(Window::glfw)) {
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
		// UPDATE PHASE 5: RENDER NEXT FRAME
		//

		// Bind framebuffer and clear buffer
		glBindFramebuffer(GL_FRAMEBUFFER, pp_fbo);
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
		glm::mat4 view = Transformation::view_matrix(renderCamera);
		glm::mat4 projection = Transformation::projection_matrix(renderCamera, Window::width, Window::height);

		// Render each linked entity
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < Runtime::entityLinks.size(); i++) {
			Runtime::entityLinks.at(i)->render(view, projection);
		}

		// Render skybox
		if (activeSkybox != nullptr) {
			activeSkybox->draw(view, projection);
		}

		// Render framebuffer

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		pp_shader->bind();
		pp_shader->setVec2("screenResolution", glm::vec2(Window::width, Window::height));

		pp_shader->setFloat("contrast", contrast);

		pp_shader->setBool("chromaticAberration", chromaticAberration);
		pp_shader->setFloat("chromaticAberrationStrength", chromaticAberrationStrength);
		pp_shader->setFloat("chromaticAberrationRange", chromaticAberrationRange);
		pp_shader->setFloat("chromaticAberrationRedOffset", chromaticAberrationRedOffset);
		pp_shader->setFloat("chromaticAberrationBlueOffset", chromaticAberrationBlueOffset);

		pp_shader->setBool("vignette", vignette);
		pp_shader->setVec4("vignetteColor", vignetteColor);
		pp_shader->setFloat("vignetteRadius", vignetteRadius);
		pp_shader->setFloat("vignetteSoftness", vignetteSoftness);
		pp_shader->setFloat("vignetteRoundness", vignetteRoundness);

		glBindVertexArray(pp_vao);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, pp_texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//
		// UPDATE PHASE 6: RENDER ENGINE UI
		//

		EngineUI::newFrame();

		// Inspector mode ui
		if (Runtime::inspectorMode) {
			if (Runtime::showEngineUI) {
				EngineDialog::vec3_dialog("Camera Position", Runtime::renderCamera->position);
				EngineDialog::vec3_dialog("Camera Rotation", Runtime::renderCamera->rotation, -360.0f, 360.0f);
				EngineDialog::float_dialog("FOV", Runtime::renderCamera->fov, 30, 90);
				EngineDialog::bool_dialog("Wireframe", Runtime::wireframe);

				InputPair a = { "Contrast", contrast, 0.95f, 1.1f };
				InputPair b = { "Chromatic Aberration Strength", chromaticAberrationStrength, 0.0f, 5.0f };
				InputPair c = { "Chromatic Aberration Range", chromaticAberrationRange, 0.0f, 1.0f };
				InputPair d = { "Chromatic Aberration Red Offset", chromaticAberrationRedOffset, -0.1f, 0.1f };
				InputPair e = { "Chromatic Aberration Blue Offset", chromaticAberrationBlueOffset, -0.1f, 0.1f };
				InputPair f = { "Vignette Radius", vignetteRadius, 0.0f, 1.0f };
				InputPair g = { "Vignette Softness", vignetteSoftness, 0.0f, 1.0f };
				InputPair h = { "Vignette Roundness", vignetteRoundness, 0.0f, 2.0f };
				EngineDialog::input_dialog("Post Processing", { a, b, c, d, e, f, g, h });

				EngineDialog::bool_dialog("Chromatic Aberration", chromaticAberration);
				EngineDialog::bool_dialog("Vignette", vignette);

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
		// UPDATE PHASE 7: MANAGE FRAME BUFFER AND WINDOW CONTEXT AND PROCESS EVENTS
		//

		glfwSwapBuffers(Window::glfw);
		glfwPollEvents();
	}

	// Exit application

	glfwTerminate();
	return 0;
}