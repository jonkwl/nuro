#include "project_assets.h"

#include <chrono>

#include <utils/console.h>

#include "../assetsys/texture_asset.h"
#include "../assetsys/asset_extensions.h"

AssetID ProjectAssets::load(const fs::path& relativePath)
{
	// Create metadata file if it doesn't exist already
	// ...

	AssetID id = generateId();
	// (Use id from the assets metadata if existing already later on)

	// Create asset
	auto optAsset = assetInstance(id, relativePath);
	if (!optAsset) return 0;

	// Register asset
	id = (*optAsset)->_assetId;
	assets[id] = (*optAsset);

	return id;
}

void ProjectAssets::unload(AssetID id)
{
	auto it = assets.find(id);
	if (it != assets.end())
		assets.erase(it);
}

bool ProjectAssets::reload(AssetID id)
{
	// Reload an asset...
	return true;
}

std::optional<AssetRef> ProjectAssets::get(AssetID id) const
{
	auto it = assets.find(id);
	if (it != assets.end())
		return it->second;
	
	return std::nullopt;
}

AssetID ProjectAssets::generateId()
{
	static uint32_t counter = 0;
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = now.time_since_epoch();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	AssetID id = milliseconds + (++counter);
	return id;
}

std::optional<AssetRef> ProjectAssets::assetInstance(AssetID id, const fs::path& relativePath)
{
	// Empty asset reference
	AssetRef asset = nullptr;

	// Resolve asset type
	AssetType type = AssetExtensions::resolveType(relativePath);

	switch (type) {
	case AssetType::TEXTURE:
		asset = std::make_shared<TextureAsset>();
		break;
	default:
		return std::nullopt;
	}

	if (!asset) return std::nullopt;

	asset->_assetId = id;
	asset->_assetType = type;
	asset->_assetPath = relativePath;

	return asset;
}