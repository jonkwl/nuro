#include "runtime.h"

std::vector<Entity*> Runtime::entityLinks;
Camera* Runtime::renderCamera = new Camera();
Camera* Runtime::activeCamera = new Camera();
Camera* Runtime::inspectorCamera = new Camera();

float Runtime::time = 0.0f;
float Runtime::lastTime = 0.0f;
float Runtime::deltaTime = 0.0f;
int Runtime::fps = 0;

bool Runtime::wireframe = true;

bool Runtime::inspectorMode = true;
bool Runtime::showEngineUI = false;
bool Runtime::showDiagnostics = false;

Entity* Runtime::createEntity() {
	Entity* entity = new Entity();
	entityLinks.push_back(entity);
	return entity;
}

void Runtime::useCamera(Camera* camera) {
	activeCamera = camera;
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

int main() {
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

	// Check for fullscreen
	if (Context::fullscreen) {
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		Context::width = mode->width;
		Context::height = mode->height;
	}

	// Create window
	Context::window = glfwCreateWindow(Context::width, Context::height, Context::title.c_str(), nullptr, nullptr);
	glfwSetFramebufferSizeCallback(Context::window, Context::framebuffer_size_callback);
	if (Context::window == nullptr) {
		return throw_err("Creation of window failed");
	}

	// Load graphics api
	glfwMakeContextCurrent(Context::window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return throw_err("Initialization of GLAD failed");
	}

	// Update context
	Context::set_viewport();
	Context::setCursor(Context::cursorMode);	

	// Setup render settings
	if (Runtime::wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1); // V-Sync

	Log::printProcessDone("Runtime", "Context created");

	//
	// SETUP PHASE 2: LOAD ASSETS, COMPILE SHADERS
	//

	// Loading all shaders
	std::vector<std::string> shader_paths = { "./resources/shaders" };
	ShaderBuilder::loadAndCompile(shader_paths);
	Shader* default_shader = ShaderBuilder::get("unlit");

	//
	// SETUP PHASE 3: CALL ANY OTHER SCRIPTS NEEDING SETUP
	//

	EngineUI::setup();
	Input::setupInputs();

	//
	// SETUP PHASE 4 (FINAL): AWAKE GAME LOGIC
	//
	awake();

	while (!glfwWindowShouldClose(Context::window)) {
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

		// Clear screen buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Select camera to be rendered (depending on whether inspector mode is activated)
		if (!Runtime::inspectorMode) {
			Runtime::renderCamera = Runtime::activeCamera;
		}
		else {
			Runtime::renderCamera = Runtime::inspectorCamera;
			InspectorMode::refreshInspector();
		}

		// Render each linked entity
		for (int i = 0; i < Runtime::entityLinks.size(); i++) {
			Entity* entity = Runtime::entityLinks.at(i);

			if (entity->model == nullptr) continue;

			// Calculate mvp
			glm::mat4 mvp = RenderCore::mvp(entity, Runtime::renderCamera, Context::width, Context::height);

			// Draw call
			entity->model->render(mvp);
		}

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
		// UPDATE PHASE 7: MANAGE WINDOW CONTEXT AND PROCESS GRAPHICS API EVENTS
		//

		glfwSwapBuffers(Context::window);
		glfwPollEvents();
	}

	// Exit application

	glfwTerminate();
	return 0;
}