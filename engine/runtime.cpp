#include "runtime.h"

std::vector<Entity*> Runtime::entityLinks;
Camera* Runtime::renderCamera = new Camera();
Camera* Runtime::activeCamera = new Camera();
Camera* Runtime::inspectorCamera = new Camera();

float Runtime::time = 0.0f;
float Runtime::lastTime = 0.0f;
float Runtime::deltaTime = 0.0f;
int Runtime::fps = 0;

bool Runtime::wireframe = false;

bool Runtime::inspectorMode = false;
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

int triangle_vao(unsigned int* indice_count) {
	// define vertices and indices
	unsigned int vertice_components = 9;
	/*float vertices[] = {
		// position (vec3)      // base_color (vec4)       // texture coords
		0.5f,  -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
		-0.5f, -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
		0.5f,   0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3,
	};*/
	float vertices[] = {
		// Position (x, y, z)     // Color (r, g, b, a)   // Texture coords (u, v)

		// Front face
		0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 0
	   -0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 1
		0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 2
	   -0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 3
	   
	    // Back face
	    0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 4
	   -0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 5
	    0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 6
	   -0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 7
	  
	    // Left face
	   -0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 8 (same as Vertex 1)
	   -0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 9 (same as Vertex 5)
	   -0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 10 (same as Vertex 3)
	   -0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 11 (same as Vertex 7)

	    // Right face
	    0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 12 (same as Vertex 0)
	    0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 13 (same as Vertex 4)
	    0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 14 (same as Vertex 2)
	    0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 15 (same as Vertex 6)

	    // Top face
	    0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 16 (same as Vertex 0)
	   -0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Vertex 17 (same as Vertex 1)
	    0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 18 (same as Vertex 4)
	   -0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 19 (same as Vertex 5)

	    // Bottom face
	    0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Vertex 20 (same as Vertex 2)
       -0.5f, -0.5f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // Vertex 21 (same as Vertex 3)
	    0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Vertex 22 (same as Vertex 6)
       -0.5f, -0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f  // Vertex 23 (same as Vertex 7)
	};

	unsigned int indices[] = {
		// Front face
		0, 1, 2,    // Triangle 1
		1, 2, 3,    // Triangle 2

		// Back face
		4, 5, 6,    // Triangle 3
		5, 6, 7,    // Triangle 4

		// Left face
		8, 9, 10,   // Triangle 5
		9, 10, 11,  // Triangle 6

		// Right face
		12, 13, 14, // Triangle 7
		13, 14, 15, // Triangle 8

		// Top face
		16, 17, 18, // Triangle 9
		17, 18, 19, // Triangle 10

		// Bottom face
		20, 21, 22, // Triangle 11
		21, 22, 23  // Triangle 12
	};
	*indice_count = sizeof(indices) / sizeof(indices[0]);

	// create vertex array object (vao)
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create vertex buffer object (vbo) and set data
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set vertex attrib pointers of vao using data from vbo
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// create element buffer object (ebo)
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// unbind vao, vbo and ebo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return vao;
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

	Log::printProcessStart("Runtime", "Loading assets...");

	// Cube vao
	unsigned int indice_count;
	unsigned int vao = triangle_vao(&indice_count);

	// Loading all shaders
	std::vector<std::string> shader_paths = { "./resources/shaders" };
	ShaderBuilder::loadAndCompile(shader_paths);
	Shader* default_shader = ShaderBuilder::get("unlit");

	Log::printProcessDone("Runtime", "Loaded assets");

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

			// Bind vao
			glBindVertexArray(vao);

			// Set shader
			// default_shader->use();
			entity->material->bind();

			// Set texture
			// entity->texture->use();

			// Calculate mvp
			glm::mat4 mvp = RenderCore::mvp(entity, Runtime::renderCamera, Context::width, Context::height);

			// Set shader uniforms
			default_shader->setMatrix4("mvp", mvp);

			// Draw call
			glDrawElements(GL_TRIANGLES, indice_count, GL_UNSIGNED_INT, 0);
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