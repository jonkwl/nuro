#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stb_image.h>

#include "../engine/rendering/shader/Shader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

using namespace std;

struct _context {
	int x = 0;
	int y = 0;
	int width = 800;
	int height = 800;
	string title = "Rendering Engine";

	GLFWwindow* window = nullptr;

	bool wireframe = false;

	double time = 0.0;
	double last_time = 0.0;
	double delta_time = 0.0;

	float camera_fov = 45;
	glm::vec3 camera_position;
	glm::vec3 camera_rotation;

	float movement_speed = 10.0f;
	float rotation_speed = 0.25f;
};
_context context;

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int throw_err(string message) {
	cout << "ERROR: " << message << endl;
	glfwTerminate();
	return -1;
}

void gl_viewport_set() {
	glViewport(context.x, context.y, context.width, context.height);
}

void process_inputs() {
	if (glfwGetKey(context.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(context.window, true);
	}

	if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS) {
		context.camera_position.z += context.movement_speed * (float)context.delta_time;
	} else if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS) {
		context.camera_position.z -= context.movement_speed * (float)context.delta_time;
	}

	if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS) {
		context.camera_position.x += context.movement_speed * (float)context.delta_time;
	}
	else if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS) {
		context.camera_position.x -= context.movement_speed * (float)context.delta_time;
	}

	if (glfwGetKey(context.window, GLFW_KEY_T) == GLFW_PRESS) {
		context.camera_position.y += context.movement_speed * (float)context.delta_time;
	}
	else if (glfwGetKey(context.window, GLFW_KEY_G) == GLFW_PRESS) {
		context.camera_position.y -= context.movement_speed * (float)context.delta_time;
	}

	if (glfwGetKey(context.window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		context.camera_rotation.y += context.rotation_speed * 10 * (float)context.delta_time;
	}
	else if (glfwGetKey(context.window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		context.camera_rotation.y -= context.rotation_speed * 10 * (float)context.delta_time;
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
	unsigned char* data = stbi_load("assets/textures/plank.jpg", &width, &height, &channels, 0);
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

void vec3_dialog(string name, glm::vec3& vec, float min = -10.0f, float max = 10.0f) {
	ImGui::Begin(string("Vector3: " + name).c_str());

	ImGui::SliderFloat("X", &vec.x, min, max);
	ImGui::SliderFloat("Y", &vec.y, min, max);
	ImGui::SliderFloat("Z", &vec.z, min, max);

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

int main() {
	glfwSetErrorCallback(glfw_error_callback);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	context.window = glfwCreateWindow(context.width, context.height, context.title.c_str(), nullptr, nullptr);
	glfwSetFramebufferSizeCallback(context.window, framebuffer_size_callback);

	if (context.window == nullptr) {
		return throw_err("Creation of window failed");
	}

	glfwMakeContextCurrent(context.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return throw_err("Initialization of GLAD failed");
	}

	gl_viewport_set();

	Shader default_shader("assets/tmp/shaders/vertex.shader", "assets/tmp/shaders/fragment.shader");
	unsigned int indice_count;
	unsigned int vao = triangle_vao(&indice_count);
	unsigned int texture = getTexture();

	if (context.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	setup_ui();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	context.last_time = glfwGetTime();

	context.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);

	while (!glfwWindowShouldClose(context.window)) {
		// set time
		context.time = glfwGetTime();
		context.delta_time = context.time - context.last_time;
		context.last_time = context.time;

		// clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// process inputs
		process_inputs();

		// set shader
		default_shader.use();

		// set texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// render objects
		glBindVertexArray(vao);
		
		glm::vec3 objects[] = {
			glm::vec3(0.0f, 0.0f, 5.0f),
			glm::vec3(-2.0f, 0.0f, 5.0f),
			glm::vec3(2.0f, 0.0f, 5.0f),
			glm::vec3(-2.0f, 0.0f, 7.0f),
			glm::vec3(2.0f, 0.0f, 7.0f)
		};

		for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
			float angle = 20.0f * i + (float)context.time * 10;

			// calculate new camera transform

			// calculate mvp
			glm::mat4 model = model_matrix(objects[i], glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			glm::mat4 view = view_matrix();
			glm::mat4 projection = projection_matrix();

			// set shader
			default_shader.setMatrix4("mvp", mvp(model, view, projection));

			glDrawElements(GL_TRIANGLES, indice_count, GL_UNSIGNED_INT, 0);
		}

		// ui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// custom ui
		vec3_dialog("Camera Position", context.camera_position);
		vec3_dialog("Camera Rotation", context.camera_rotation, -360.0f, 360.0f);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// handle events and buffer
		glfwSwapBuffers(context.window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}