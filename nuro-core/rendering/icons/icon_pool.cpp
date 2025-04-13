#include "icon_pool.h"

#include <vector>
#include <stb_image.h>
#include <unordered_map>

#include <utils/console.h>
#include <memory/resource_manager.h>
#include <rendering/texture/texture.h>
#include <context/application_context.h>

namespace IconPool {

	// List of valid extensions for icon source files
	std::vector<FS::Path> gValidExtensions = { ".png", ".jpg", ".jpeg" };

	// Global registry of all icons by their identifiers
	std::unordered_map<std::string, ResourceRef<Texture>> gIcons;

	// Texture of fallback icon for invalid icons
	ResourceRef<Texture> gInvalidIcon;

	void _loadAll(const FS::Path& directory, bool async)
	{
		ResourceManager& resource = ApplicationContext::resourceManager();

		std::vector<FS::Path> files = FS::getFilesWithExtensions(directory, gValidExtensions);
		for (const auto& file : files) {
			// Get icon identifier
			std::string identifier = file.stem().string();

			// Skip icon creation if it already exists
			if (gIcons.find(identifier) != gIcons.end()) continue;

			// Get icon texture type
			TextureType type = file.extension() == ".png" ? TextureType::IMAGE_RGBA : TextureType::IMAGE_RGB;

			// Create icon texture
			auto& [textureId, texture] = resource.create<Texture>(identifier + "_icon");
			texture->setSource(type, file);

			// Insert new icon
			gIcons.insert({ identifier, texture });

			// Load icon texture
			if (async) {
				resource.exec(texture->create());
			}
			else {
				resource.execAsDependency(texture->create());
			}

		}
	}

	void loadAllSync(const FS::Path& directory)
	{
		Console::out::info("Icon Pool", "Loading icons from '" + directory.string() + "'");
		_loadAll(directory, false);
	}

	void loadAllAsync(const FS::Path& directory)
	{
		Console::out::info("Icon Pool", "Queued loading icons in '" + directory.string() + "'");
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

	void createFallbackIcon(const FS::Path& path)
	{
		ResourceManager& resource = ApplicationContext::resourceManager();

		auto& [textureId, texture] = resource.create<Texture>("fallback-icon");
		gInvalidIcon = texture;
		gInvalidIcon->setSource(TextureType::IMAGE_RGBA, path);

		resource.execAsDependency(texture->create());
	}

}