#include "project_assets.h"

#include <chrono>

#include <utils/console.h>

#include "../runtime/runtime.h"
#include "../assetsys/texture_asset.h"
#include "../reflection/asset_registry.h"

AssetID ProjectAssets::load(const FS::Path& relativePath)
{
	FS::Path absolutePath = FS::Path(Runtime::projectManager().project().path) / relativePath;

	// Try to fetch asset info
	auto assetInfo = AssetRegistry::fetchByPath(relativePath);
	if (!assetInfo) 
		return 0;

	// Touch metadata file
	FS::Path metaPath = absolutePath.string() + ".meta";
	if (!FS::touch(metaPath))
		return 0;

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

void ProjectAssets::remove(AssetID id)
{
	// Try to find asset id
	auto it = assets.find(id);
	if (it == assets.end())
		return;
	AssetRef asset = it->second;

	// Cache assets path
	FS::Path relativePath = asset->_assetPath;
	FS::Path absolutePath = FS::Path(Runtime::projectManager().project().path) / relativePath;

	// Unloads asset and removes it
	it->second->onUnload();
	assets.erase(it);

	// Delete assets metafile if existing
	FS::Path metaPath = absolutePath.string() + ".meta";
	if (FS::exists(metaPath))
		FS::remove(metaPath);
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