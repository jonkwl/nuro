#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FileReader {
public:
	static std::string Read(std::string path) {
		std::ifstream fileStream(path);
		if (!fileStream.is_open()) {
			std::cerr << "File Reader | Failed to open file: " << path << std::endl;
			return "";
		}
		std::stringstream buffer;
		buffer << fileStream.rdbuf();
		fileStream.close();
		return buffer.str();
	}
};