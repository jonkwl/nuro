#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include <resource/resource.h>

class Shader : public Resource
{
protected:
	bool loadIoData() override;
	void freeIoData() override;
	bool uploadBuffers() override;
	void deleteBuffers() override;

public:
	Shader();
	~Shader();

	std::string sourcePath() override;

	// Sets the path of the shaders source
	void setSource(std::string path);

	// Binds the shader program
	void bind() const;

	// Returns the shader programs backend id
	uint32_t backendId() const;

	void setBool(const std::string& identifier, bool value);
	void setInt(const std::string& identifier, int32_t value);
	void setFloat(const std::string& identifier, float value);
	void setVec2(const std::string& identifier, glm::vec2 value);
	void setVec3(const std::string& identifier, glm::vec3 value);
	void setVec4(const std::string& identifier, glm::vec4 value);
	void setMatrix3(const std::string& identifier, glm::mat3 value);
	void setMatrix4(const std::string& identifier, glm::mat4 value);

private:
	struct Data {
		std::string vertexSource;
		std::string fragmentSource;
	};

	// Path of shader source
	std::string path;

	// Shader source data
	Data data;

	// Shader program uniform location cache
	std::unordered_map<std::string, int32_t> uniforms;

	// Shader program backend id
	uint32_t _backendId;

private:
	int32_t getUniformLocation(const std::string& identifier);

	bool shaderCompiled(const char* type, int32_t shader);
	bool programLinked(int32_t program);
};
