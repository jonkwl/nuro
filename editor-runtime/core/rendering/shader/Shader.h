#pragma once

#include <cstdint>
#include <glm.hpp>
#include <string>
#include <unordered_map>

class Shader
{
public:
	explicit Shader(const std::string& name);

	bool compile(const char* vertexSource, const char* fragmentSource);

	void bind();

	inline uint32_t getId() const { 
		return id; 
	}

	inline const std::string& getName() const { 
		return name; 
	}

	void setBool(const std::string& identifier, bool value);
	void setInt(const std::string& identifier, int32_t value);
	void setFloat(const std::string& identifier, float value);
	void setVec2(const std::string& identifier, glm::vec2 value);
	void setVec3(const std::string& identifier, glm::vec3 value);
	void setVec4(const std::string& identifier, glm::vec4 value);
	void setMatrix3(const std::string& identifier, glm::mat3 value);
	void setMatrix4(const std::string& identifier, glm::mat4 value);

private:
	uint32_t id;
	std::string name;
	std::unordered_map<std::string, int32_t> uniforms;

	int32_t getUniformLocation(const std::string& identifier);
	bool shaderCompiled(const char* type, int32_t shader);
	bool programLinked(int32_t program);
};
