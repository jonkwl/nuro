#include "Shader.h"

Shader::Shader(std::string vertex_path, std::string fragment_path) {
	std::string log;

	std::string str_vertex_code = FileReader::Read(vertex_path);
	std::string str_fragment_code = FileReader::Read(fragment_path);

	const char* vertex_code = str_vertex_code.c_str();
	const char* fragment_code = str_fragment_code.c_str();

	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_code, nullptr);
	glCompileShader(vertex);
	if (!shader_compiled(std::string("Vertex"), vertex, &log)) {
		std::cerr << log << std::endl;
		return;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_code, nullptr);
	glCompileShader(fragment);
	if (!shader_compiled(std::string("Fragment"), fragment, &log)) {
		std::cerr << log << std::endl;
		return;
	}

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	if (!program_linked(id, &log)) {
		std::cerr << log << std::endl;
		return;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
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

bool Shader::shader_compiled(std::string type, int shader, std::string* log) {
	int success;
	char shader_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, shader_log);
		std::string _log(shader_log);
		*log = "Shader Compilation Error / " + type + " | " + _log;
		return false;
	}
	return true;
}

bool Shader::program_linked(int program, std::string* log) {
	int success;
	char shader_log[512];
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, shader_log);
		std::string _log(shader_log);
		*log = "Program Link Error | " + _log;
		return false;
	}
	return true;
}