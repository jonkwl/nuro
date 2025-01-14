#pragma once

#include <cstdint>
#include <glm.hpp>
#include <string>
#include <unordered_map>

class Shader
{
public:
	explicit Shader(const char* vertex_src, const char* fragment_src, bool& compiled, const std::string name);

	void bind();

	uint32_t getId() { return id; }
	std::string getName() { return shaderName; }

	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int32_t value);
	void setFloat(const std::string& name, float value);
	void setVec2(const std::string& name, glm::vec2 value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setMatrix3(const std::string& name, glm::mat3 value);
	void setMatrix4(const std::string& name, glm::mat4 value);

	// Inserts the given array index in some uniform identifier, will insert array index between the [] in the given identifier
	static std::string uniformArray(const std::string& identifier, uint32_t arrayIndex);

private:
	uint32_t id;
	std::string shaderName;
	std::unordered_map<std::string, int32_t> uniformCache;

	int32_t getUniformLocation(const std::string& name);
	bool shader_compiled(std::string type, int32_t shader);
	bool program_linked(int32_t program);
};
