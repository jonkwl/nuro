#include "cubemap.h"

#include <stb_image.h>
#include <glad/glad.h>
#include <filesystem>

#include <utils/console.h>
#include <utils/ioutils.h>

namespace fs = std::filesystem;

void Cubemap::loadData()
{
	switch (source.type) {
	case Source::Type::CROSS:
		if (source.paths.empty()) return;
		loadCrossCubemap(source.paths[0]);
		break;
	case Source::Type::INDIVIDUAL:
		if (source.paths.size() < 6) return;
		loadIndividualFace(source.paths[0]);
		loadIndividualFace(source.paths[1]);
		loadIndividualFace(source.paths[2]);
		loadIndividualFace(source.paths[3]);
		loadIndividualFace(source.paths[4]);
		loadIndividualFace(source.paths[5]);
		break;
	default:
		break;
	}
}

void Cubemap::releaseData()
{
	data.clear();
}

void Cubemap::dispatchGPU()
{
	// Don't dispatch cubemap if there is no data
	if (data.empty()) return;

	glGenTextures(1, &_backendId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _backendId);

	for (int32_t i = 0; i < data.size(); i++)
	{
		FaceData face = data[i];

		GLenum format = GL_RGB;
		if (face.channels == 4)
		{
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, face.width, face.height, 0, format, GL_UNSIGNED_BYTE, face.data.data());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::Cubemap() : source(),
data(),
_backendId(0)
{
}

void Cubemap::setSource_Cross(std::string path)
{
	// Validate source path
	if (!fs::exists(path)) {
		Console::out::warning("Cubemap", "Cubemap source at '" + path + "' could not be found");
	}

	source.type = Source::Type::CROSS;
	source.paths = { path };
}

void Cubemap::setSource_Individual(std::string rightPath, std::string leftPath, std::string topPath, std::string bottomPath, std::string frontPath, std::string backPath)
{
	// Source path validation lambda
	auto validatePath = [](const std::string& path, const std::string& face) {
		if (!fs::exists(path)) {
			Console::out::warning("Cubemap", "Cubemap source for '" + face + "' face at '" + path + "' could not be found");
		}
	};

	// Validate source paths
	validatePath(rightPath, "right");
	validatePath(leftPath, "left");
	validatePath(topPath, "top");
	validatePath(bottomPath, "bottom");
	validatePath(frontPath, "front");
	validatePath(backPath, "back");

	source.type = Source::Type::INDIVIDUAL;
	source.paths = { rightPath, leftPath, topPath, bottomPath, frontPath, backPath };
}

uint32_t Cubemap::backendId() const
{
	return _backendId;
}

std::string Cubemap::sourcePath()
{
	if (source.paths.size() > 0) {
		return IOUtils::getFilename(source.paths[0]);
	}
	else {
		return "";
	}
}

Cubemap::ImageData Cubemap::loadImageData(std::string path)
{
	stbi_set_flip_vertically_on_load(false);

	int32_t width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		Console::out::warning("Cubemap", "Failed to load cubemap at " + path);
		return ImageData();
	}

	return ImageData(data, width, height, channels);
}

void Cubemap::loadCrossCubemap(std::string path)
{
	//
	// WORK ON MEMORY EFFICIENCY HERE!
	//
	
	// Load temporary image data
	ImageData image = loadImageData(path);

	// Calculate dimensions of each face (assuming default 4x3 layout)
	int32_t faceWidth = image.width / 4;
	int32_t faceHeight = image.height / 3;

	// Resize data vector to hold 6 faces
	data.resize(6);

	// Define offsets for each face
	std::vector<std::pair<int32_t, int32_t>> offsets = {
		{2 * faceWidth, 1 * faceHeight}, // Positive X (Right)
		{0 * faceWidth, 1 * faceHeight}, // Negative X (Left)
		{1 * faceWidth, 0 * faceHeight}, // Positive Y (Top)
		{1 * faceWidth, 2 * faceHeight}, // Negative Y (Bottom)
		{1 * faceWidth, 1 * faceHeight}, // Positive Z (Front)
		{3 * faceWidth, 1 * faceHeight}  // Negative Z (Back)
	};

	// Extract pixel data for each face
	for (size_t i = 0; i < offsets.size(); ++i)
	{
		int32_t xOffset = offsets[i].first;
		int32_t yOffset = offsets[i].second;

		// Allocate memory for face data
		data[i].data.resize(faceWidth * faceHeight * image.channels);
		data[i].width = faceWidth;
		data[i].height = faceHeight;
		data[i].channels = image.channels;

		// Copy pixel data for each face
		for (int32_t y = 0; y < faceHeight; ++y)
		{
			for (int32_t x = 0; x < faceWidth; ++x)
			{
				int32_t srcIndex = ((yOffset + y) * image.width + (xOffset + x)) * image.channels;
				int32_t dstIndex = (y * faceWidth + x) * image.channels;
				memcpy(&data[i].data[dstIndex], &image.data[srcIndex], image.channels);
			}
		}
	}

	// Free temporary image data
	stbi_image_free(image.data);
}

void Cubemap::loadIndividualFace(std::string path)
{
	//
	// WORK ON MEMORY EFFICIENCY HERE!
	// Can be optimized using move semantics etc
	//

	// Load temporary image data
	ImageData image = loadImageData(path);

	// Initialize face data
	std::vector<unsigned char> faceData(image.data, image.data + (image.width * image.height * image.channels));

	// Create face
	FaceData face;
	face.data = faceData;
	face.width = image.width;
	face.height = image.height;
	face.channels = image.channels;
	data.push_back(face);

	// Free temporary image data
	stbi_image_free(image.data);
}