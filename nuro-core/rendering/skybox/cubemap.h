#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <utils/fsutil.h>
#include <memory/resource.h>

class Cubemap : public Resource
{
public:
	Cubemap();
	~Cubemap() override;

	// Default pipe for creating cubemap
	ResourcePipe create() {
		return std::move(pipe()
			>> BIND_TASK(Cubemap, loadIoData)
			>> BIND_TASK_WITH_FLAGS(Cubemap, uploadBuffers, TaskFlags::UseContextThread));
	}

	// Sets cubemaps source to be cross layout
	void setSource_Cross(const FS::Path& sourcePath);

	// Sets cubemaps source to be multiple individual faces
	void setSource_Individual(const FS::Path& rightPath, const FS::Path& leftPath, const FS::Path& topPath, const FS::Path& bottomPath, const FS::Path& frontPath, const FS::Path& backPath);

	// Returns the backend id of the cubemap texture
	uint32_t backendId() const;

private:
	struct Source {
		enum class Type {
			CROSS,
			INDIVIDUAL
		};

		// Type of the cubemap source
		Type type = Type::CROSS;

		// All image source paths associated with the cubemap
		std::vector<FS::Path> paths;
	};

	struct FaceData
	{
		std::vector<unsigned char> data;
		int32_t width = 0;
		int32_t height = 0;
		int32_t channels = 0;
	};

	struct ImageData
	{
		unsigned char* data;
		int32_t width;
		int32_t height;
		int32_t channels;

		ImageData() :
			data(nullptr),
			width(0),
			height(0),
			channels(0)
		{};

		ImageData(unsigned char* data, uint32_t width, uint32_t height, uint32_t channels) :
			data(data),
			width(width),
			height(height),
			channels(channels)
		{};
	};

private:
	// Loads a cubemap by multiple faces in one file
	void loadCrossCubemap(const FS::Path& sourcePath);

	// Loads an individual face for a cubemap
	void loadIndividualFace(const FS::Path& sourcePath);

	bool loadIoData();
	void freeIoData();
	bool uploadBuffers();
	void deleteBuffers();

	// Cubemap source
	Source source;

	// Cubemap data
	std::vector<FaceData> data;

	// Backend id of cubemap texture
	uint32_t _backendId;

	// Loads given images data
	ImageData loadImageData(const FS::Path& sourcePath);
};