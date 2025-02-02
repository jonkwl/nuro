#include "icon_pool.h"

#include <vector>
#include <unordered_map>
#include <stb_image.h>

#include "../core/utils/iohandler.h"
#include "../core/utils/console.h"

namespace IconPool {

	std::vector<std::string> gValidExtensions = { ".png", ".jpg", ".jpeg" };
	std::unordered_map<std::string, Texture> gIcons;
	Texture gInvalidIcon = Texture::empty();

	void load(const std::string& directoryPath)
	{
		Console::out::processStart("Gizmo Icon Pool", "Loading icons from " + directoryPath);

		uint32_t nLoaded = 0;
		std::vector<std::string> files = IOHandler::getFilesWithExtensions(directoryPath, gValidExtensions);
		for (const auto& file : files) {
			std::string filename = IOHandler::getFilenameRaw(file);
			TextureType type = IOHandler::getFileExtension(file) == ".png" ? TextureType::IMAGE_RGBA : TextureType::IMAGE_RGB;
			gIcons.insert({ filename, Texture::load(file, type) });
			nLoaded++;
		}

		Console::out::processDone("Gizmo Icon Pool", "Done: " + std::to_string(nLoaded) + (nLoaded == 1 ? " icon loaded" : " icons loaded"));
	}

	Texture& get(const std::string& identifier)
	{
		auto it = gIcons.find(identifier);
		if (it != gIcons.end()) {
			return it->second;
		}
		else {
			return gInvalidIcon;
		}
	}

}