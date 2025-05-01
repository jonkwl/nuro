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

AssetSID ProjectAssets::load(const FS::Path& relativePath)
{
	FS::Path absolutePath = FS::Path(Runtime::projectManager().project().path) / relativePath;

	// Try to fetch asset info
	auto assetInfo = AssetRegistry::fetchByPath(relativePath);
	if (!assetInfo) 
		return 0;

	// Create asset instance
	auto asset = assetInfo->createInstance();
	asset->_assetType = assetInfo->type;
	asset->_assetPath = relativePath;
	asset->_assetKey.sessionID = createSID();

	//
	// METADATA
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

	// Register asset instance
	assets[asset->_assetKey.sessionID] = asset;

	// Register asset guid and session id link
	assetSIDs[asset->_assetKey.guid] = asset->_assetKey.sessionID;

	// Asset default load event
	asset->onDefaultLoad(metaPath);

	return asset->_assetKey.sessionID;
}

void ProjectAssets::remove(AssetSID id)
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

bool ProjectAssets::reload(AssetSID id)
{
	// Reload an asset...
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

AssetSID ProjectAssets::createSID()
{
	return ++sidCounter;
}