#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>
#include "../../utils/filereader.h"

class Shader
{
public:
	unsigned int id;

	Shader(std::string vertex_path, std::string fragment_path);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
private:
	static bool shader_compiled(int shader, std::string* log);
	static bool program_linked(int program, std::string* log);
};

