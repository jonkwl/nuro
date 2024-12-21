#include "cubemap.h"

#include <stb_image.h>

#include "../core/utils/log.h"
#include "../core/utils/iohandler.h"

Cubemap Cubemap::loadByCubemap(std::string cubemapPath)
{
	Cubemap cubemap;
	cubemap.name = IOHandler::getFilenameRaw(cubemapPath);

	Log::printProcessStart("Cubemap", "Generating cubemap " + cubemap.name + "...");

	cubemap.loadCubemapFaces(cubemapPath);

	Log::printProcessDone("Cubemap", "Cubemap generated");

	return cubemap;
}

Cubemap Cubemap::loadByFaces(std::string rightFacePath, std::string leftFacePath, std::string topFacePath, std::string bottomFacePath, std::string frontFacePath, std::string backFacePath)
{
	Cubemap cubemap;
	cubemap.name = IOHandler::getLastFolder(rightFacePath);

	Log::printProcessStart("Cubemap", "Generating cubemap " + cubemap.name + "...");

	cubemap.loadFace(rightFacePath);
	cubemap.loadFace(leftFacePath);
	cubemap.loadFace(topFacePath);
	cubemap.loadFace(bottomFacePath);
	cubemap.loadFace(frontFacePath);
	cubemap.loadFace(backFacePath);

	Log::printProcessDone("Cubemap", "Cubemap generated");

	return cubemap;
}

Image Cubemap::loadImage(std::string path)
{
	stbi_set_flip_vertically_on_load(false);

	int32_t width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		Log::printError("Cubemap", "Failed to load cubemap at " + path);
		return Image();
	}

	return Image{ width, height, channels, data };
}

void Cubemap::loadFace(std::string facePath)
{
	Image image = loadImage(facePath);

	std::vector<unsigned char> faceData(image.data, image.data + (image.width * image.height * image.channels));

	CubemapFace face;
	face.data = faceData;
	face.width = image.width;
	face.height = image.height;
	face.channels = image.channels;
	faces.push_back(face);
}

void Cubemap::loadCubemapFaces(std::string cubemapPath)
{
	Image image = loadImage(cubemapPath);

	// Calculate dimensions of each face (assuming default 4x3 layout)
	int32_t faceWidth = image.width / 4;
	int32_t faceHeight = image.height / 3;

	// Resize faces vector to hold 6 faces
	faces.resize(6);

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
		faces[i].data.resize(faceWidth * faceHeight * image.channels);
		faces[i].width = faceWidth;
		faces[i].height = faceHeight;
		faces[i].channels = image.channels;

		// Copy pixel data for each face
		for (int32_t y = 0; y < faceHeight; ++y)
		{
			for (int32_t x = 0; x < faceWidth; ++x)
			{
				int32_t srcIndex = ((yOffset + y) * image.width + (xOffset + x)) * image.channels;
				int32_t dstIndex = (y * faceWidth + x) * image.channels;
				memcpy(&faces[i].data[dstIndex], &image.data[srcIndex], image.channels);
			}
		}
	}

	// Free loaded image data
	stbi_image_free(image.data);
}