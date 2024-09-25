#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>
#include "../../utils/filereader.h"
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int id;

	Shader(std::string vertex_path, std::string fragment_path);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix4(const std::string& name, glm::mat4 value) const;
private:
	static bool shader_compiled(std::string type, int shader, std::string* log);
	static bool program_linked(int program, std::string* log);
};

