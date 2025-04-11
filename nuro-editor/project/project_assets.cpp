#include "project_assets.h"

#include <chrono>

#include <utils/console.h>
#include <utils/fsutil.h>

#include "../assetsys/texture_asset.h"
#include "../reflection/asset_registry.h"

namespace fs = std::filesystem;

AssetID ProjectAssets::load(const fs::path& relativePath)
{
	// Try to fetch asset info
	auto assetInfo = AssetRegistry::fetchByPath(relativePath);
	if (!assetInfo) return 0;

	// Create metadata file if it doesn't exist already
	// ...

	AssetID id = generateId();
	// (Use id from the assets metadata if existing already later on)

	// Create instance of asset
	auto asset = assetInfo->createInstance();
	asset->_assetId = id;
	asset->_assetType = assetInfo->type;
	asset->_assetPath = relativePath;
	asset->onDefaultLoad();

	// Register asset
	assets[id] = asset;

	return id;
}

void ProjectAssets::unload(AssetID id)
{
	auto it = assets.find(id);
	if (it == assets.end())
		return;

	it->second->onUnload();
	assets.erase(it);
}

bool ProjectAssets::reload(AssetID id)
{
	// Reload an asset...
	return true;
}

AssetRef ProjectAssets::get(AssetID id) const
{
	auto it = assets.find(id);
	if (it != assets.end()) return it->second;
	return nullptr;
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