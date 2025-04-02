#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include <filesystem>
namespace fs = std::filesystem;

#include "../assetsys/editor_asset.h"

using AssetRef = std::shared_ptr<EditorAsset>;

class ProjectAssets {
public:
	// Loads an editor asset and returns its id
	AssetID load(const fs::path& relativePath);

	// Unloads an editor asset if existing
	void unload(AssetID id);

	// Reloads an asset, returns success
	bool reload(AssetID id);

	// Returns an editor asset reference by its id
	std::optional<AssetRef> get(AssetID id) const;

private:
	// Generates an id for an editor asset
	AssetID generateId();

	// Creates an editor asset instance
	std::optional<AssetRef> assetInstance(AssetID id, const fs::path& relativePath);

	// Registry of all assets by their id
	std::unordered_map<AssetID, AssetRef> assets;

};