#include "icon_pool.h"

#include <vector>
#include <unordered_map>
#include <stb_image.h>

#include <utils/console.h>
#include <utils/iohandler.h>
#include <rendering/texture/texture.h>
#include <context/application_context.h>

namespace IconPool {

	// List of valid extensions for icon source files
	std::vector<std::string> gValidExtensions = { ".png", ".jpg", ".jpeg" };

	// Global registry of all icons
	std::unordered_map<std::string, Texture*> gIcons;

	// Texture of fallback icon for invalid icons
	Texture* gInvalidIcon = new Texture();

	void _loadAll(const std::string& directory, bool async)
	{
		ResourceLoader& loader = ApplicationContext::getResourceLoader();

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

		}
	}

	void loadAllSync(const std::string& directory)
	{
		Console::out::processState("Icon Pool", "Loading icons from '" + directory + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const std::string& directory)
	{
		Console::out::processState("Icon Pool", "Queued loading icons in '" + directory + "'");
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

	void createFallbackIcon(const std::string& path)
	{
		gInvalidIcon->setSource(TextureType::IMAGE_RGBA, path);
		ApplicationContext::getResourceLoader().createSync(gInvalidIcon);
	}

}