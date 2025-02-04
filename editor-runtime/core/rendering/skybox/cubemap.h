#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../core/resource/resource.h"

class Cubemap : public Resource
{
public:
	Cubemap();

	// Sets cubemaps source to be cross layout
	void setSource_Cross(std::string path);

	// Sets cubemaps source to be multiple individual faces
	void setSource_Individual(std::string rightPath, std::string leftPath, std::string topPath, std::string bottomPath, std::string frontPath, std::string backPath);

	// Returns the backend id of the cubemap texture
	uint32_t getId() const;

	void loadData() override;
	void releaseData() override;
	void dispatchGPU() override;
	std::string sourcePath() override;

private:
	struct Source {
		enum class Type {
			CROSS,
			INDIVIDUAL
		};

		// Type of the cubemap source
		Type type;

		// All image source paths associated with the cubemap
		std::vector<std::string> paths;
	};

	struct Face
	{
		std::vector<unsigned char> data;
		int32_t width;
		int32_t height;
		int32_t channels;
	};

	struct Image
	{
		unsigned char* data;
		int32_t width;
		int32_t height;
		int32_t channels;
	};

private:
	// Cubemap source
	Source source;

	// All loaded cubemap faces
	std::vector<Face> faces;

	// Backend id of cubemap texture
	uint32_t id;

	// Loads given images data
	Image loadImageData(std::string path);

	// Loads a cubemap by multiple faces in one file
	void loadCrossCubemap(std::string path);

	// Loads an individual face for a cubemap
	void loadIndividualFace(std::string path);
};