#pragma once

#include <glm.hpp>
#include <string>
#include <unordered_map>

class Shader
{
public:
	explicit Shader(const char *vertex_src, const char *fragment_src, bool &compiled, const std::string name);

	void bind();

	unsigned int getId() { return id; }
	std::string getName() { return shaderName; }

	void setBool(const std::string &name, bool value);
	void setInt(const std::string &name, int value);
	void setFloat(const std::string &name, float value);
	void setVec2(const std::string &name, glm::vec2 value);
	void setVec3(const std::string &name, glm::vec3 value);
	void setVec4(const std::string &name, glm::vec4 value);
	void setMatrix3(const std::string &name, glm::mat3 value);
	void setMatrix4(const std::string &name, glm::mat4 value);

private:
	unsigned int id;
	std::string shaderName;
	std::unordered_map<std::string, int> uniformCache;

	int getUniformLocation(const std::string &name);
	bool shader_compiled(std::string type, int shader);
	bool program_linked(int program);
};
