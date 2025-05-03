#include "asset_registry.h"

#include <string>
#include <unordered_map>

#include "../assetsys/font_asset.h"
#include "../assetsys/editor_asset.h"
#include "../assetsys/texture_asset.h"
#include "../assetsys/fallback_asset.h"
#include "../ui/inspectables/inspectable.h"
#include "../ui/windows/insight_panel_window.h"
#include "../ui/inspectables/asset_inspectable.h"

namespace AssetRegistry {

    // Asset info for fallback asset
    std::shared_ptr<AssetInfo> fallbackAsset;

    // Global registry for all assets by extension
    std::unordered_map<std::string, std::shared_ptr<AssetInfo>> gAssetRegistry;

    template <typename InstanceType>
    std::shared_ptr<AssetInfo> createAssetInfo(AssetType type) {
        static_assert(std::is_base_of<EditorAsset, InstanceType>::value, "Only classes that derive from EditorAsset are valid for asset registration");

        // Create asset info instance dynamically
        auto assetInfo = std::make_shared<AssetInfo>();
        assetInfo->type = type;
        assetInfo->createInstance = []() { return std::make_shared<InstanceType>(); };
        assetInfo->inspect = [](AssetSID id) { InsightPanelWindow::inspect<AssetInspectable>(id); };

        return assetInfo;
    }

    // Registers an asset onto the registry
    template <typename InstanceType>
    void registerAsset(AssetType type, std::vector<std::string> extensions){
        // Create asset info
        auto assetInfo = createAssetInfo<InstanceType>(type);

        // Register asset info reference by extensions
        for (const std::string& extension : extensions) {
            gAssetRegistry[extension] = assetInfo;
        }
    }

    const std::unordered_map<std::string, std::shared_ptr<AssetInfo>>& get()
    {
        return gAssetRegistry;
    }

    std::shared_ptr<AssetInfo> fetchByPath(const FS::Path& path)
    {
        // Extract file extension from path
        std::string extension = path.extension().string();
        if (!extension.empty() && extension[0] == '.')
            extension.erase(0, 1);

        // Find asset info by extension in registry
        auto it = gAssetRegistry.find(extension);
        if (it == gAssetRegistry.end()) {
            if (extension != "meta")
                return fallbackAsset;
            return nullptr;
        }
        return it->second;
    }

    void create()
    {
        fallbackAsset = createAssetInfo<FallbackAsset>(AssetType::FALLBACK);

        registerAsset<TextureAsset>(
            AssetType::TEXTURE,
            { "jpg", "jpeg", "png", "bmp", "tga", "psd", "gif", "hdr", "pic", "ppm", "pgm" }
        );

        registerAsset<FontAsset>(
            AssetType::FONT,
            { "ttf", "otf" }
        );
    }

}