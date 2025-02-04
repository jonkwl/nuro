#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct CubemapFace
{
	std::vector<unsigned char> data;
	int32_t width;
	int32_t height;
	int32_t channels;
};

struct Image
{
	int32_t width;
	int32_t height;
	int32_t channels;
	unsigned char* data;
};

class Cubemap
{
public:
	Cubemap(std::string name);

	void load(std::string cubemapPath);
	void load(std::string rightFacePath, std::string leftFacePath, std::string topFacePath, std::string bottomFacePath, std::string frontFacePath, std::string backFacePath);

	std::string name;
	std::vector<CubemapFace> faces;

private:
	Image loadImage(std::string path);
	void loadSingularFace(std::string facePath);
	void loadCubemapFaces(std::string cubemapPath);
};