#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <functional>

#include <utils/fsutil.h>

#include "../assetsys/editor_asset.h"

struct AssetInfo {

	using AssetRef = std::shared_ptr<EditorAsset>;
	using AssetID = uint32_t;

	// Type of the asset
	AssetType type = AssetType::FALLBACK;

	// Function to create an asset instance from this asset
	std::function<AssetRef()> createInstance;

	// Function to inspect this asset type by an assets id
	std::function<void(AssetID)> inspect;

};

namespace AssetRegistry {

	// Returns asset registry (key represents file extension)
	const std::unordered_map<std::string, std::shared_ptr<AssetInfo>>& get();

	// Fetches asset info by an asset path, nullptr if none
	std::shared_ptr<AssetInfo> fetchByPath(const FS::Path& path);

	// Creates global asset registry
	void create();

}