#include "icon_pool.h"

#include <vector>
#include <unordered_map>
#include <stb_image.h>

#include "../core/utils/iohandler.h"
#include "../core/utils/console.h"

namespace IconPool {

	std::vector<std::string> _validExtensions = { ".png", ".jpg", ".jpeg" };

	std::unordered_map<std::string, Texture> icons;
	Texture invalidIcon = Texture::empty();

	void loadAll(const std::string& directoryPath)
	{
		Console::out::processStart("Gizmo Icon Pool", "Loading icons from " + directoryPath);

		uint32_t nLoaded = 0;
		std::vector<std::string> files = IOHandler::getFilesWithExtensions(directoryPath, _validExtensions);
		for (const auto& file : files) {
			std::string filename = IOHandler::getFilenameRaw(file);
			TextureType type = IOHandler::getFileExtension(file) == ".png" ? TextureType::IMAGE_RGBA : TextureType::IMAGE_RGB;
			icons.insert({ filename, Texture::load(file, type) });
			nLoaded++;
		}

		Console::out::processDone("Gizmo Icon Pool", "Done: " + std::to_string(nLoaded) + (nLoaded == 1 ? " icon loaded" : " icons loaded"));
	}

	Texture& get(const std::string& identifier)
	{
		auto it = icons.find(identifier);
		if (it != icons.end()) {
			return it->second;
		}
		else {
			return invalidIcon;
		}
	}

}