#pragma once

#include <string>
#include <vector>
#include <stb_image.h>

#include "../src/utils/log.h"
#include "../src/utils/iohandler.h"

struct CubemapFace {
	std::vector<unsigned char> data;
	int width;
	int height;
	int channels;
};

struct Image {
	int width;
	int height;
	int channels;
	unsigned char* data;
};

class Cubemap
{
public:
	static Cubemap* GetBySingle(std::string path);
	static Cubemap* GetByFaces(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);

	std::string name;
	std::vector<CubemapFace> faces;
private:
	Image loadImage(std::string path);
	void extractFaces(std::string path);
	void loadFace(std::string path);
};

