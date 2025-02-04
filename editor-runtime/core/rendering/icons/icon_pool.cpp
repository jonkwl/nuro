#include "icon_pool.h"

#include <vector>
#include <unordered_map>
#include <stb_image.h>

#include "../core/utils/console.h"
#include "../core/utils/iohandler.h"
#include "../core/rendering/texture/texture.h"

namespace IconPool {

	std::vector<std::string> gValidExtensions = { ".png", ".jpg", ".jpeg" };
	std::unordered_map<std::string, Texture*> gIcons;
	Texture* gInvalidIcon = new Texture();

	void createFallback(const std::string& invalidIconPath)
	{
		gInvalidIcon->setSource(TextureType::IMAGE_RGBA, invalidIconPath);
		gInvalidIcon->createSync();
	}

	void load(const std::string& directoryPath)
	{
		Console::out::processStart("Icon Pool", "Loading icons from " + directoryPath);

		uint32_t nLoaded = 0;
		std::vector<std::string> files = IOHandler::getFilesWithExtensions(directoryPath, gValidExtensions);
		for (const auto& file : files) {

			std::string filename = IOHandler::getFilenameRaw(file);
			TextureType type = IOHandler::getFileExtension(file) == ".png" ? TextureType::IMAGE_RGBA : TextureType::IMAGE_RGB;

			auto [it, inserted] = gIcons.insert({ filename, new Texture() });
			auto& icon = it->second;
			icon->setSource(type, file);
			icon->createSync();

			nLoaded++;

		}

		Console::out::processDone("Icon Pool", "Done: " + std::to_string(nLoaded) + (nLoaded == 1 ? " icon loaded" : " icons loaded"));
	}

	uint32_t get(const std::string& identifier)
	{
		auto it = gIcons.find(identifier);
		if (it != gIcons.end()) {
			return it->second->getId();
		}
		else {
			Console::out::warning("Icon Pool", "Icon '" + identifier + "' was requested but isn't valid");
			return gInvalidIcon->getId();
		}
	}

}