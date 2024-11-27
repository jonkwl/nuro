#include "shader.h"

#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

#include "../../utils/log.h"
#include "../../utils/iohandler.h"

Shader::Shader(const char *vertex_src, const char *fragment_src, bool &compiled, const std::string name):
	id(0),
	shaderName(name),
	uniformCache()
{
	shaderName = name;

	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_src, nullptr);
	glCompileShader(vertex);
	if (!shader_compiled(std::string("vertex"), vertex))
	{
		compiled = false;
		return;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_src, nullptr);
	glCompileShader(fragment);
	if (!shader_compiled(std::string("fragment"), fragment))
	{
		compiled = false;
		return;
	}

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	if (!program_linked(id))
	{
		compiled = false;
		return;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	compiled = true;
}

void Shader::bind()
{
	glUseProgram(id);
}

void Shader::setBool(const std::string &name, bool value)
{
	glUniform1i(getUniformLocation(name), (int)value);
}
void Shader::setInt(const std::string &name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}
void Shader::setFloat(const std::string &name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}
void Shader::setVec2(const std::string &name, glm::vec2 value)
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}
void Shader::setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}
void Shader::setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix3(const std::string &name, glm::mat3 value)
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setMatrix4(const std::string &name, glm::mat4 value)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

int Shader::getUniformLocation(const std::string &name)
{
	if (uniformCache.find(name) != uniformCache.end())
	{
		return uniformCache[name];
	}

	int location = glGetUniformLocation(id, name.c_str());
	uniformCache[name] = location;

	return location;
}

bool Shader::shader_compiled(std::string type, int shader)
{
	int success;
	char shader_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, shader_log);
		Log::printError("Shader", "Compilation of " + type + " shader failed!", shader_log);
		return false;
	}
	return true;
}

bool Shader::program_linked(int program)
{
	int success;
	char shader_log[512];
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, shader_log);
		Log::printError("Shader", "Linking of program failed!", shader_log);
		return false;
	}
	return true;
}