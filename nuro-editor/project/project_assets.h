#pragma once

#include <memory>
#include <unordered_map>

#include <utils/fsutil.h>

#include "../assetsys/editor_asset.h"

using AssetRef = std::shared_ptr<EditorAsset>;

class ProjectAssets {
public:
	ProjectAssets();

	// Loads an editor asset and returns its session id
	AssetSID load(const FS::Path& relativePath);

	// Removes an editor asset if existing
	void remove(AssetSID id);

	// Reloads an asset, returns success
	bool reload(AssetSID id);

	// Returns an editor asset reference by its session id, nullptr if none
	AssetRef get(AssetSID id) const;

	// Returns an assets session id by its guid
	AssetSID resolveGUID(AssetGUID guid);

private:
	// Counter for asset session ids
	uint32_t sidCounter;

	// Returns a new session id
	AssetSID createSID();

	// Registry of all assets by their session id
	std::unordered_map<AssetSID, AssetRef> assets;

	// Registry of all asset session ids by their guid
	std::unordered_map<AssetGUID, AssetSID> assetSIDs;

};