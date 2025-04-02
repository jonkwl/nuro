#pragma once

#include <string>
#include <unordered_map>

#include <filesystem>
namespace fs = std::filesystem;

enum class AssetType;

class AssetExtensions {
public:
    // Resolves the type of an asset by its path
    static AssetType resolveType(const fs::path& assetPath);

private:
    static std::unordered_map<std::string, AssetType> extensionMap;
};