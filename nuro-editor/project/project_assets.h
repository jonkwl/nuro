#pragma once

#include <memory>
#include <unordered_map>

#include <utils/fsutil.h>

#include "../assetsys/editor_asset.h"

using AssetRef = std::shared_ptr<EditorAsset>;

class ProjectAssets {
public:
	// Loads an editor asset and returns its id
	AssetID load(const FS::Path& relativePath);

	// Removes an editor asset if existing
	void remove(AssetID id);

	// Reloads an asset, returns success
	bool reload(AssetID id);

	// Returns an editor asset reference by its id, nullptr if none
	AssetRef get(AssetID id) const;

private:
	// Generates an id for an editor asset
	AssetID generateId();

	// Registry of all assets by their id
	std::unordered_map<AssetID, AssetRef> assets;

};