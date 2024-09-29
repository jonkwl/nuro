#pragma once

#include <glad/glad.h>
#include <string>
#include "../../utils/log.h"
#include "../../utils/iohandler.h"
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(const char* vertex_src, const char* fragment_src, bool& compiled);

	void bind();

	unsigned int getId() { return id; }

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
	void setMatrix4(const std::string& name, glm::mat4 value) const;
private:
	unsigned int id;

	static bool shader_compiled(std::string type, int shader);
	static bool program_linked(int program);
};

