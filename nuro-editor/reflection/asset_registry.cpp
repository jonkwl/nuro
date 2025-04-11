#include "asset_registry.h"

#include <string>
#include <unordered_map>

#include "../assetsys/editor_asset.h"
#include "../assetsys/texture_asset.h"
#include "../ui/inspectables/inspectable.h"
#include "../ui/windows/insight_panel_window.h"
#include "../ui/inspectables/texture_inspectable.h"

namespace fs = std::filesystem;

namespace AssetRegistry {

    // Global registry for all assets by extension
    std::unordered_map<std::string, std::shared_ptr<AssetInfo>> gAssetRegistry;

    // Registers an asset onto the registry, provide the derived editor asset type and derived inspectable type
    template <typename InstanceType, typename InspectableType>
    void registerAsset(
        AssetType type,
        std::vector<std::string> extensions
    ){
        static_assert(std::is_base_of<EditorAsset, InstanceType>::value, "Only classes that derive from EditorAsset are valid for asset registration");
        static_assert(std::is_base_of<Inspectable, InspectableType>::value, "Only classes that derive from Inspectable are valid for asset registration");

        // Create asset info instance dynamically
        auto assetInfo = std::make_shared<AssetInfo>();
        assetInfo->type = type;
        assetInfo->createInstance = []() { return std::make_shared<InstanceType>(); };
        assetInfo->inspect = [](AssetID id) { InsightPanelWindow::inspect<InspectableType>(id); };

        // Register asset info reference by extensions
        for (const std::string& extension : extensions) {
            gAssetRegistry[extension] = assetInfo;
        }
    }

    void create()
    {
        registerAsset<TextureAsset, TextureInspectable>(
            AssetType::TEXTURE,
            { "jpg", "jpeg", "png", "bmp", "tga", "psd", "gif", "hdr", "pic", "ppm", "pgm" }
        );
    }

    const std::unordered_map<std::string, std::shared_ptr<AssetInfo>>& get()
    {
        return gAssetRegistry;
    }

    std::shared_ptr<AssetInfo> fetchByPath(const fs::path& path)
    {
        // Extract file extension from path
        std::string extension = path.extension().string();
        if (!extension.empty() && extension[0] == '.')
            extension.erase(0, 1);

        // Find asset info by extension in registry
        auto it = gAssetRegistry.find(extension);
        if (it == gAssetRegistry.end()) return nullptr;
        return it->second;
    }

}