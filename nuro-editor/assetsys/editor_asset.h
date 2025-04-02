#pragma once

#include <vector>
#include <cstdint>

#include <filesystem>
namespace fs = std::filesystem;

class ProjectAssets;

using AssetID = uint32_t;

enum class AssetType {
	UNKNOWN,

	ANIMATION,
	AUDIO_CLIP,
	MATERIAL,
	MODEL,
	TEXTURE
};

class EditorAsset {
private:
	friend class ProjectAssets;

	// Assets id
	AssetID _assetId;

	// Type of the asset
	AssetType _assetType;

	// Path to asset relative to project root
	fs::path _assetPath;

	// List of assets this asset depends on
	std::vector<AssetID> _assetDependencies;

protected:
	EditorAsset() : _assetId(0), _assetType(AssetType::UNKNOWN), _assetDependencies() {};

public:
	virtual ~EditorAsset() = 0;

	// Returns the editor assets id
	AssetID id() const {
		return _assetId;
	}

	// Returns the type of the asset
	AssetType type() const {
		return _assetType;
	}

	// Returns the path to the asset relative to project root path
	fs::path path() const {
		return _assetPath;
	}

	// Returns the editor assets dependencies
	const std::vector<uint32_t>& dependencies() const {
		return _assetDependencies;
	}

	// Inspects the asset in the insight panel
	virtual void inspect() const = 0;

	// Returns if the asset is currently in a loading state
	virtual bool loading() const = 0;

	// Returns the assets icon
	virtual uint32_t icon() const = 0;
};

inline EditorAsset::~EditorAsset() {}