#include "shader.h"

Shader::Shader(const char* vertex_src, const char* fragment_src, bool& compiled) {
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_src, nullptr);
	glCompileShader(vertex);
	if (!shader_compiled(std::string("vertex"), vertex)) {
		compiled = false;
		return;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_src, nullptr);
	glCompileShader(fragment);
	if (!shader_compiled(std::string("fragment"), fragment)) {
		compiled = false;
		return;
	}

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	if (!program_linked(id)) {
		compiled = false;
		return;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	compiled = true;
}

void Shader::bind() {
	glUseProgram(id);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setMatrix4(const std::string& name, glm::mat4 value) const {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

bool Shader::shader_compiled(std::string type, int shader) {
	int success;
	char shader_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, shader_log);
		Log::printError("Shader", "Compilation of " + type + " shader failed!");
		Log::printProcessInfo(shader_log);
		return false;
	}
	return true;
}

bool Shader::program_linked(int program) {
	int success;
	char shader_log[512];
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, shader_log);
		Log::printError("Shader", "Linking of program failed!");
		Log::printProcessInfo(shader_log);
		return false;
	}
	return true;
}