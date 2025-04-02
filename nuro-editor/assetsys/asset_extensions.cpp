#include "asset_extensions.h"

#include "../assetsys/editor_asset.h"

std::unordered_map<std::string, AssetType> AssetExtensions::extensionMap = {

    //
    // TEXTURE FORMATS
    //

    {"png", AssetType::TEXTURE},  
    {"jpg", AssetType::TEXTURE},
    {"jpeg", AssetType::TEXTURE}, 
    {"bmp", AssetType::TEXTURE},
    {"tga", AssetType::TEXTURE},

    //
    // MODEL FORMATS
    //
    
    {"obj", AssetType::MODEL},
    {"fbx", AssetType::MODEL},    
    {"gltf", AssetType::MODEL},
    {"glb", AssetType::MODEL},

    //
    // SOUND FORMATS
    //

    {"wav", AssetType::AUDIO_CLIP},
    {"mp3", AssetType::AUDIO_CLIP},    
    {"ogg", AssetType::AUDIO_CLIP},
    {"flac", AssetType::AUDIO_CLIP}

};

AssetType AssetExtensions::resolveType(const fs::path& assetPath)
{
    std::string extension = assetPath.extension().string();
    if (!extension.empty() && extension[0] == '.')
        extension.erase(0, 1);

    auto it = extensionMap.find(extension);
    return (it != extensionMap.end()) ? it->second : AssetType::UNKNOWN;
}