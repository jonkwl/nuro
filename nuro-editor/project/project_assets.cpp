#include "project_assets.h"

#include <chrono>

#include <utils/console.h>

#include "../runtime/runtime.h"
#include "../assetsys/asset_meta.h"
#include "../assetsys/texture_asset.h"
#include "../reflection/asset_registry.h"

ProjectAssets::ProjectAssets() : sidCounter(0),
assets(),
assetSIDs()
{
}

AssetSID ProjectAssets::load(const FS::Path& path)
{
	FS::Path absolutePath = Runtime::projectManager().abs(path);

	//
	// PREPARE ASSET
	//

	// Try to fetch asset type info
	auto assetInfo = AssetRegistry::fetchByPath(path);
	if (!assetInfo) 
		return 0;

	// Create asset instance
	AssetRef asset = assetInfo->createInstance();
	asset->_assetType = assetInfo->type;
	asset->_assetPath = path;
	asset->_assetKey.sessionID = createSID();

	//
	// LOAD OR CREATE METADATA
	//

	FS::Path metaPath = absolutePath.string() + ".meta";

	// Metadata doesn't exist yet, create metadata header
	if (!FS::exists(metaPath)) {
		asset->_assetKey.guid = XG::createGUID();

		// Create metadata file
		if (!FS::touch(metaPath))
			return 0;

		// Write metadata header
		if (!FS::writeFile(metaPath, AssetMeta::createHeader(asset->_assetKey.guid)))
			return 0;
	}
	// Metadata exists already, fetch guid from metadata
	else {
		asset->_assetKey.guid = AssetMeta::parseGUID(metaPath);
	}

	// Ensure guid was parsed or generated
	if (!asset->_assetKey.guid.isValid()) 
		return 0;

	//
	// REGISTER ASSET
	//

	// Register asset instance
	assets[asset->_assetKey.sessionID] = asset;

	// Register asset guid and session id link
	assetSIDs[asset->_assetKey.guid] = asset->_assetKey.sessionID;

	//
	// ASSET LOAD EVENT
	//

	// Asset default load event
	asset->onDefaultLoad(metaPath);

	return asset->_assetKey.sessionID;
}

void ProjectAssets::remove(AssetSID id)
{
	// Try to find asset
	auto it = assets.find(id);
	if (it == assets.end())
		return;
	AssetRef asset = it->second;

	// Cache assets path
	FS::Path absolutePath = Runtime::projectManager().abs(asset->_assetPath);

	// Unloads asset and removes it
	asset->onUnload();
	assets.erase(it);

	// Delete assets metafile if existing
	FS::Path metaPath = absolutePath.string() + ".meta";
	if (FS::exists(metaPath))
		FS::remove(metaPath);
}

bool ProjectAssets::reload(AssetSID id)
{
	// Try to find asset
	auto it = assets.find(id);
	if (it == assets.end())
		return false;
	AssetRef asset = it->second;

	// Reload asset
	asset->onReload();
	return true;
}

AssetRef ProjectAssets::get(AssetSID id) const
{
	auto it = assets.find(id);
	if (it != assets.end()) 
		return it->second;
	return nullptr;
}

AssetSID ProjectAssets::resolveGUID(AssetGUID guid)
{
	auto it = assetSIDs.find(guid);
	if (it != assetSIDs.end())
		return it->second;
	return 0;
}

void ProjectAssets::updateLocation(AssetSID id, FS::Path path)
{
	auto it = assets.find(id);
	if (it == assets.end())
		return;
	AssetRef asset = it->second;

	// Cache old relative path and update it
	FS::Path oldRelativePath = asset->_assetPath;
	asset->_assetPath = path;

	// Cache old and current absolute path
	FS::Path oldAbsolutePath = Runtime::projectManager().abs(oldRelativePath);
	FS::Path absolutePath = Runtime::projectManager().abs(path);

	// Moves the meta file
	FS::rename(oldAbsolutePath.string() + ".meta", absolutePath.string() + ".meta");
}

AssetSID ProjectAssets::createSID()
{
	return ++sidCounter;
}