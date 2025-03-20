#include "icon_pool.h"

#include <vector>
#include <unordered_map>
#include <stb_image.h>

#include <utils/console.h>
#include <utils/ioutils.h>
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
		ResourceManager& resource = ApplicationContext::resourceManager();

		std::vector<std::string> files = IOUtils::getFilesWithExtensions(directory, gValidExtensions);
		for (const auto& file : files) {
			// Get icon identifier
			std::string identifier = IOUtils::getFilenameRaw(file);

			// Skip icon creation if it already exists
			if (gIcons.find(identifier) != gIcons.end()) continue;

			// Get icon texture type
			TextureType type = IOUtils::getFileExtension(file) == ".png" ? TextureType::IMAGE_RGBA : TextureType::IMAGE_RGB;

			// Insert new icon
			auto [_, texture] = resource.create<Texture>(identifier);
			auto [it, inserted] = gIcons.insert({ identifier, texture });
			auto& icon = it->second;

			// Load icon texture
			icon->setSource(type, file);
			if (async) {
				resource.loadAsync(icon);
			}
			else {
				resource.loadSync(icon);
			}

		}
	}

	void loadAllSync(const std::string& directory)
	{
		Console::out::info("Icon Pool", "Loading icons from '" + directory + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const std::string& directory)
	{
		Console::out::info("Icon Pool", "Queued loading icons in '" + directory + "'");
		_loadAll(directory, true);
	}

	uint32_t get(const std::string& identifier)
	{
		if (auto it = gIcons.find(identifier); it != gIcons.end()) {
			// Icon found, return its backend id
			return it->second->backendId();
		}
		else {
			// Icon not found, return invalid icons backend id
			return gInvalidIcon->backendId();
		}
	}

	void createFallbackIcon(const std::string& path)
	{
		ResourceManager& resource = ApplicationContext::resourceManager();

		auto [_, invalidIcon] = resource.create<Texture>("fallback-icon");
		gInvalidIcon = invalidIcon;
		gInvalidIcon->setSource(TextureType::IMAGE_RGBA, path);

		resource.loadSync(gInvalidIcon);
	}

}