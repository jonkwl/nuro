#include "icon_pool.h"

#include <vector>
#include <unordered_map>
#include <stb_image.h>

#include "../src/core/utils/console.h"
#include "../src/core/utils/iohandler.h"
#include "../src/core/rendering/texture/texture.h"
#include "../src/core/context/application_context.h"

namespace IconPool {

	std::vector<std::string> gValidExtensions = { ".png", ".jpg", ".jpeg" };
	std::unordered_map<std::string, Texture*> gIcons;
	Texture* gInvalidIcon = new Texture();

	void _loadAll(const std::string& directory, bool async)
	{
		ResourceLoader& loader = ApplicationContext::getResourceLoader();
		uint32_t nLoaded = 0;

		std::vector<std::string> files = IOHandler::getFilesWithExtensions(directory, gValidExtensions);
		for (const auto& file : files) {
			// Get icon identifier
			std::string identifier = IOHandler::getFilenameRaw(file);

			// Skip icon creation if it already exists
			if (gIcons.find(identifier) != gIcons.end()) continue;

			// Get icon texture type
			TextureType type = IOHandler::getFileExtension(file) == ".png" ? TextureType::IMAGE_RGBA : TextureType::IMAGE_RGB;

			// Insert new icon
			auto [it, inserted] = gIcons.insert({ identifier, new Texture()});
			auto& icon = it->second;

			// Create icon texture
			icon->setSource(type, file);
			if (async) {
				loader.createAsync(icon);
			}
			else {
				loader.createSync(icon);
			}

			nLoaded++;

		}

		Console::out::processDone("Icon Pool", "Loaded " + std::to_string(nLoaded) + (nLoaded == 1 ? " icon" : " icons"));
	}

	void createFallbackIconSync(const std::string& invalidIconPath)
	{
		gInvalidIcon->setSource(TextureType::IMAGE_RGBA, invalidIconPath);
		ApplicationContext::getResourceLoader().createSync(gInvalidIcon);
	}

	void loadAllSync(const std::string& directory)
	{
		Console::out::processStart("Icon Pool", "Loading icons from '" + directory + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const std::string& directory)
	{
		Console::out::processStart("Icon Pool", "Queued loading icons in '" + directory + "'");
		_loadAll(directory, true);
	}

	uint32_t get(const std::string& identifier)
	{
		if (auto it = gIcons.find(identifier); it != gIcons.end()) {
			// Icon found, return its backend id
			return it->second->id();
		}
		else {
			// Icon not found, return invalid icons backend id
			// Console::out::warning("Icon Pool", "Icon '" + identifier + "' was requested but isn't valid");
			return gInvalidIcon->id();
		}
	}

}