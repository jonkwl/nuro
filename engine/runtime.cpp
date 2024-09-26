#include "runtime.h"

std::vector<Entity*> entity_links;
Camera* active_camera;

struct _context {
	// Window
	int x = 0;
	int y = 0;
	int width = 800;
	int height = 800;
	std::string title = "Rendering Engine";
	bool fullscreen = true;
	GLFWwindow* window = nullptr;

	// Render settings
	bool wireframe = false;

	// Time
	float time = 0.0;
	float last_time = 0.0;
	float delta_time = 0.0;

	// Environmental settings
	glm::vec4 background_color;

	// Cursor settings
	GLenum cursor_mode = GLFW_CURSOR_DISABLED;
	glm::vec2 mouse_last;

	// Camera settings
	float camera_fov = 70;
	glm::vec3 camera_position;
	glm::vec3 camera_rotation;

	// Movement settings
	float movement_speed = 12.0f;
	float sensitivity = 6.0f;
	bool lock_movement = false;
	bool lock_lookaround = false;

	// Input settings
	float key_smooth_factor = 5.0f;
	glm::vec2 key_axis;
	glm::vec2 key_axis_smooth;
	glm::vec2 mouse_axis;

	// Input cache
	bool esc_pressed = false;
	bool show_ui = false;
};
_context context;

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int throw_err(std::string message) {
	std::cout << "ERROR: " << message << std::endl;
	glfwTerminate();
	return -1;
}

void gl_viewport_set() {
	glViewport(context.x, context.y, context.width, context.height);
}

void update_cursor() {
	glfwSetInputMode(context.window, GLFW_CURSOR, context.cursor_mode);
}

void process_inputs() {
	if (glfwGetKey(context.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(context.window, true);
	}

	if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS) {
		context.key_axis.x = 1.0f;
	} else if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS) {
		context.key_axis.x = -1.0f;
	}
	else {
		context.key_axis.x = 0.0f;
	}

	if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS) {
		context.key_axis.y = 1.0f;
	}
	else if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS) {
		context.key_axis.y = -1.0f;
	}
	else {
		context.key_axis.y = 0.0f;
	}

	if (glfwGetKey(context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (context.esc_pressed) return;
		context.esc_pressed = true;
		context.show_ui = !context.show_ui;
		if (context.cursor_mode == GLFW_CURSOR_NORMAL) {
			context.cursor_mode = GLFW_CURSOR_DISABLED;
		}
		else {
			context.cursor_mode = GLFW_CURSOR_NORMAL;
		}
		update_cursor();
		context.lock_lookaround = !context.lock_lookaround;
	}
	else {
		context.esc_pressed = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	context.width = width;
	context.height = height;
	gl_viewport_set();
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

unsigned int getTexture() {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("./user/assets/textures/plank.jpg", &width, &height, &channels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}

void setup_ui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(context.window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void vec3_dialog(std::string name, glm::vec3& value, float min = -10.0f, float max = 10.0f) {
	ImGui::Begin(std::string("Vector3: " + name).c_str());

	ImGui::SliderFloat("X", &value.x, min, max);
	ImGui::SliderFloat("Y", &value.y, min, max);
	ImGui::SliderFloat("Z", &value.z, min, max);

	ImGui::End();
}

void float_dialog(std::string name, float& value, float min = -10.0f, float max = 10.0f) {
	ImGui::Begin(std::string("Float:" + name).c_str());

	ImGui::SliderFloat(name.c_str(), &value, min, max);

	ImGui::End();
}

void bool_dialog(std::string name, bool& value, float min = -10.0f, float max = 10.0f) {
	ImGui::Begin(std::string("Bool:" + name).c_str());

	ImGui::Checkbox(name.c_str(), &value);

	ImGui::End();
}

void color_dialog(std::string name, glm::vec4& value) {
	ImGui::Begin(std::string("Bool:" + name).c_str());

	ImGui::ColorPicker4(name.c_str(), (float*)&value);

	ImGui::End();
}

glm::mat4 model_matrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	glm::mat4 model(1.0f);

	// object position
	model = glm::translate(model, glm::vec3(position.x, position.y, -position.z));

	// object rotation
	model = glm::rotate(model, glm::radians(-rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	// object scale
	model = glm::scale(model, scale);

	return model;
}

glm::mat4 view_matrix() { 
	/*glm::vec3 camera_position = context.camera_position;
	camera_position = glm::vec3(camera_position.x, camera_position.y, -camera_position.z);

	return glm::lookAt(camera_position, camera_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/

	glm::vec3 camera_position = context.camera_position;
	glm::vec3 camera_rotation = context.camera_rotation;

	camera_position = glm::vec3(camera_position.x, camera_position.y, -camera_position.z);
	camera_rotation = glm::vec3(-camera_rotation.x, -camera_rotation.y, camera_rotation.z);

	glm::vec3 radRotation = glm::radians(camera_rotation);

	// Create rotation quaternions
	glm::quat pitchQuat = glm::angleAxis(radRotation.x, glm::vec3(1, 0, 0)); // Pitch (X)
	glm::quat yawQuat = glm::angleAxis(radRotation.y, glm::vec3(0, 1, 0));   // Yaw (Y)
	glm::quat rollQuat = glm::angleAxis(radRotation.z, glm::vec3(0, 0, 1));  // Roll (Z)

	// Combine rotations
	glm::quat orientation = yawQuat * pitchQuat * rollQuat;

	// Convert quaternion to matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(orientation);

	// Calculate the forward direction
	glm::vec3 forward = rotationMatrix * glm::vec4(0, 0, -1, 0); // Forward vector
	glm::vec3 up = rotationMatrix * glm::vec4(0, 1, 0, 0);       // Up vector

	// Calculate the target position
	glm::vec3 target = camera_position + forward;

	// Create the view matrix
	glm::mat4 viewMatrix = glm::lookAt(camera_position, target, up);

	return viewMatrix;
}

glm::mat4 projection_matrix() {
	return glm::perspective(glm::radians(context.camera_fov), (float)context.width / (float)context.height, 0.1f, 100.0f);
}

glm::mat4 mvp(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	return projection * view * model;
}

glm::vec3 vec_forward(const glm::vec3& rotation) {
	// Convert Euler angles (in degrees) to radians
	glm::vec3 rotation_rad = glm::radians(rotation);

	// Create a quaternion from the Euler angles
	glm::quat quaternion = glm::quat(rotation_rad);

	// Define the forward vector in local space (typically negative Z-axis)
	glm::vec3 forward_local = glm::vec3(0.0f, 0.0f, 1.0f); // Points forward in local space

	// Rotate the local forward vector by the quaternion
	glm::vec3 forward_vector = quaternion * forward_local;

	// Return the forward vector
	return forward_vector;
}

glm::vec3 vec_right(const glm::vec3& rotation) {
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 right_local = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right_vector = quaternion * right_local;
	return right_vector;
}

void refresh_key_axis_smooth() {
	context.key_axis_smooth = glm::mix(context.key_axis_smooth, context.key_axis, context.key_smooth_factor * context.delta_time);
}

void calculate_camera_movement() {
	if (!context.lock_movement) {
		glm::vec3 cam_forward = vec_forward(context.camera_rotation);
		glm::vec3 cam_right = vec_right(context.camera_rotation);
		glm::vec3 movement_direction = cam_forward * context.key_axis_smooth.x + cam_right * context.key_axis_smooth.y;
		context.camera_position += movement_direction * context.movement_speed * context.delta_time;
	}

	if (!context.lock_lookaround) {
		glm::vec3 rotate_direction = glm::vec3(-context.mouse_axis.y, context.mouse_axis.x, 0.0f);
		glm::vec3 new_rotation = context.camera_rotation + (rotate_direction * context.sensitivity * context.delta_time);
		new_rotation = glm::vec3(glm::clamp(new_rotation.x, -90.0f, 90.0f), new_rotation.y, new_rotation.z);
		context.camera_rotation = new_rotation;
	}
}

int main() {
	glfwSetErrorCallback(glfw_error_callback);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (context.fullscreen) {
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		context.width = mode->width;
		context.height = mode->height;
	}

	context.window = glfwCreateWindow(context.width, context.height, context.title.c_str(), nullptr, nullptr);
	glfwSetFramebufferSizeCallback(context.window, framebuffer_size_callback);

	if (context.window == nullptr) {
		return throw_err("Creation of window failed");
	}

	update_cursor();

	if (context.wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glfwMakeContextCurrent(context.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return throw_err("Initialization of GLAD failed");
	}

	gl_viewport_set();

	// Shader default_shader("assets/tmp/shaders/vertex.shader", "assets/tmp/shaders/fragment.shader");
	unsigned int indice_count;
	unsigned int vao = triangle_vao(&indice_count);

	std::vector<std::string> shader_paths = { "./resources/shaders" };
	std::vector<Shader*> shaders = ShaderBuilder::loadAndCompile(shader_paths);
	Shader* default_shader = shaders.at(0);

	bool uploaded;
	Texture plank("./user/assets/textures/plank.jpg", uploaded);
	Texture obama("./user/assets/textures/obama.jpg", uploaded);

	setup_ui();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	context.last_time = glfwGetTime();

	double mouseX, mouseY;
	glfwGetCursorPos(context.window, &mouseX, &mouseY);
	context.mouse_last = glm::vec2(mouseX, mouseY);

	context.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);

	// Awake game logic
	awake();

	while (!glfwWindowShouldClose(context.window)) {
		// set time
		context.time = glfwGetTime();
		context.delta_time = context.time - context.last_time;
		context.last_time = context.time;

		// set mouse
		double mouseX, mouseY;
		glfwGetCursorPos(context.window, &mouseX, &mouseY);
		context.mouse_axis = glm::vec2(mouseX - context.mouse_last.x, -(mouseY - context.mouse_last.y));
		context.mouse_last = glm::vec2(mouseX, mouseY);

		// refresh key axis smooth
		refresh_key_axis_smooth();

		// clear screen
		glClearColor(context.background_color.x, context.background_color.y, context.background_color.z, context.background_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// process inputs
		process_inputs();

		// update game logic before rendering
		update();

		// render objects
		glBindVertexArray(vao);

		calculate_camera_movement();

		for (int i = 0; i < entity_links.size(); i++) {
			Entity* entity = entity_links.at(i);

			// set shader
			default_shader->use();

			// set texture
			plank.use();

			// calculate mvp
			glm::mat4 model = model_matrix(entity->position, entity->rotation, entity->scale);
			glm::mat4 view = view_matrix();
			glm::mat4 projection = projection_matrix();

			// set shader
			default_shader->setMatrix4("mvp", mvp(model, view, projection));

			glDrawElements(GL_TRIANGLES, indice_count, GL_UNSIGNED_INT, 0);
		}

		// ui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// custom ui
		if (context.show_ui) {
			vec3_dialog("Camera Position", context.camera_position);
			vec3_dialog("Camera Rotation", context.camera_rotation, -360.0f, 360.0f);
			float_dialog("FOV", context.camera_fov, 30, 90);
			bool_dialog("Wireframe", context.wireframe);
			color_dialog("Background Color", context.background_color);

			if (context.wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// handle events and buffer
		glfwSwapBuffers(context.window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

Entity* Runtime::createEntity() {
	Entity* entity = new Entity();
	entity_links.push_back(entity);
	return entity;
}

void Runtime::useCamera(Camera* camera) {
	active_camera = camera;
}