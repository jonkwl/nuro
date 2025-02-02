#include "shader.h"

#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

#include "../../utils/console.h"
#include "../../utils/iohandler.h"

Shader::Shader(const std::string& _name) : id(0),
name(name),
uniforms()
{
	name = _name;
}

bool Shader::compile(const char* vertexSource, const char* fragmentSource)
{
	uint32_t vertex, fragment;

	// Compile vertex shader source
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, nullptr);
	glCompileShader(vertex);
	if (!shaderCompiled("vertex", vertex)) return false;

	// Compile fragment shader source
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, nullptr);
	glCompileShader(fragment);
	if (!shaderCompiled("fragment", fragment)) return false;

	// Create and link shader program
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	if (!programLinked(id)) return false;

	// Delete shader sources
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return true;
}

void Shader::bind()
{
	glUseProgram(id);
}

void Shader::setBool(const std::string& identifier, bool value)
{
	glUniform1i(getUniformLocation(identifier), (int32_t)value);
}
void Shader::setInt(const std::string& identifier, int32_t value)
{
	glUniform1i(getUniformLocation(identifier), value);
}
void Shader::setFloat(const std::string& identifier, float value)
{
	glUniform1f(getUniformLocation(identifier), value);
}
void Shader::setVec2(const std::string& identifier, glm::vec2 value)
{
	glUniform2f(getUniformLocation(identifier), value.x, value.y);
}
void Shader::setVec3(const std::string& identifier, glm::vec3 value)
{
	glUniform3f(getUniformLocation(identifier), value.x, value.y, value.z);
}
void Shader::setVec4(const std::string& identifier, glm::vec4 value)
{
	glUniform4f(getUniformLocation(identifier), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix3(const std::string& identifier, glm::mat3 value)
{
	glUniformMatrix3fv(getUniformLocation(identifier), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setMatrix4(const std::string& identifier, glm::mat4 value)
{
	glUniformMatrix4fv(getUniformLocation(identifier), 1, GL_FALSE, glm::value_ptr(value));
}

int32_t Shader::getUniformLocation(const std::string& identifier)
{
	// Uniform found in cache, return uniform location
	if (uniforms.find(identifier) != uniforms.end()) return uniforms[identifier];

	// Uniform not found in cache, fetch and save uniform location
	int32_t location = glGetUniformLocation(id, identifier.c_str());
	uniforms[identifier] = location;

	// Return newly fetched uniform location
	return location;
}

bool Shader::shaderCompiled(const char* type, int32_t shader)
{
	// Fetch shader compilation status
	int32_t success;
	char shader_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	// Shader compilation failed, terminate program
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, shader_log);
		Console::out::warning("Shader", "Compilation of " + std::string(type) + " shader failed!", shader_log);
		return false;
	}

	// Shader compilation successful
	return true;
}

bool Shader::programLinked(int32_t program)
{
	// Fetch shader program linking status
	int32_t success;
	char shader_log[512];
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);

	// Shader program linking failed, terminate program
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, shader_log);
		Console::out::warning("Shader", "Linking of program failed!", shader_log);
		return false;
	}

	// Shader program linking successful
	return true;
}