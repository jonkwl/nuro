#include "texture_inspectable.h"

#include <functional>

#include "../assetsys/texture_asset.h"
#include "../ui/components/im_components.h"

TextureInspectable::TextureInspectable(AssetID assetId) : assetId(assetId)
{
}

void TextureInspectable::renderStaticContent(ImDrawList& drawList)
{
    auto asset = std::dynamic_pointer_cast<TextureAsset>(Runtime::projectManager().assets().get(assetId));
    if (!asset)
        return;

    IMComponents::label(asset->path().filename().string(), EditorUI::getFonts().h2_bold);
    ImGui::Separator();
    float contentX = ImGui::GetContentRegionAvail().x;
    ImGui::Image(asset->icon(), ImVec2(contentX, contentX), ImVec2(0, 1), ImVec2(1, 0));
}

void TextureInspectable::renderDynamicContent(ImDrawList& drawList)
{

}