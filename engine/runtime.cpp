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

using namespace std;

struct _context {
	double time = 0.0;
	int x = 0;
	int y = 0;
	int width = 800;
	int height = 800;
	string title = "Rendering Engine";
	GLFWwindow* window = nullptr;
	bool wireframe = false;
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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	context.width = width;
	context.height = height;
	gl_viewport_set();
}

int triangle_vao(unsigned int* indice_count) {
	// define vertices and indices
	unsigned int vertice_components = 9;
	float vertices[] = {
		// position (vec3)      // base_color (vec4)       // texture coords
		0.5f,  -0.5f,  0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
		-0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f,
		0.5f,   0.5f,  0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f,  0.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3,
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

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertice_components * sizeof(float), (void*)(3*sizeof(float)));
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
	unsigned char* data = stbi_load("assets/textures/texture.jpg", &width, &height, &channels, 0);
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

void default_ui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is an example of using ImGui with OpenGL!");
	if (ImGui::Button("Click me!")) {
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		return throw_err("Initialization of GLAD failed");
	}

	gl_viewport_set();

	Shader default_shader("assets/tmp/shaders/vertex.shader", "assets/tmp/shaders/fragment.shader");
	unsigned int indice_count;
	unsigned int vao = triangle_vao(&indice_count);
	unsigned int texture = getTexture();

	if(context.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	setup_ui();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(context.window)) {
		// set time
		context.time = glfwGetTime();

		// clear screen
		glClear(GL_COLOR_BUFFER_BIT);

		// process inputs
		process_inputs();

		// set shader
		default_shader.use();

		// set texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// render objects
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indice_count, GL_UNSIGNED_INT, 0);

		// ui
		// default_ui();

		// handle events and buffer
		glfwSwapBuffers(context.window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}