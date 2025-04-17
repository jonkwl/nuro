#include "project_assets.h"

#include <chrono>

#include <utils/console.h>

#include "../runtime/runtime.h"
#include "../assetsys/asset_meta.h"
#include "../assetsys/texture_asset.h"
#include "../reflection/asset_registry.h"

// tmp
bool _safeGUIDParse(const std::string& str, uint32_t& out) {
	try {
		size_t idx = 0;
		unsigned long val = std::stoul(str, &idx, 10);

		if (idx != str.length()) {
			return false;
		}

		if (val > std::numeric_limits<uint32_t>::max()) {
			return false;
		}

		out = static_cast<uint32_t>(val);
		return true;

	}
	catch (const std::invalid_argument& e) {
		return false;
	}
	catch (const std::out_of_range& e) {
		return false;
	}
}

AssetID ProjectAssets::load(const FS::Path& relativePath)
{
	FS::Path absolutePath = FS::Path(Runtime::projectManager().project().path) / relativePath;

	// Try to fetch asset info
	auto assetInfo = AssetRegistry::fetchByPath(relativePath);
	if (!assetInfo) 
		return 0;

	//
	// METADATA
	//

	FS::Path metaPath = absolutePath.string() + ".meta";
	AssetID id = 0;

	// Metadata doesn't exist yet, create metadata header
	if (!FS::exists(metaPath)) {

		AssetID newId = generateId();
		AssetGUID newGuid = std::to_string(newId);

		// Create metadata file
		if (!FS::touch(metaPath))
			return 0;

		// Write metadata header
		if (!FS::writeFile(metaPath, AssetMeta::createHeader(newGuid)))
			return 0;

		id = newId;
	}
	// Metadata exists already, fetch guid from metadata
	else {
		AssetGUID guid = AssetMeta::parseGUID(metaPath);
		if (!_safeGUIDParse(guid, id))
			return 0;
	}

	// Ensure id was parsed or generated
	if (!id) return 0;

	// Create instance of asset
	auto asset = assetInfo->createInstance();
	asset->_assetId = id;
	asset->_assetType = assetInfo->type;
	asset->_assetPath = relativePath;
	asset->onDefaultLoad(metaPath);

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