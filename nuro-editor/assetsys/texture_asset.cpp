#include "texture_asset.h"

#include <rendering/icons/icon_pool.h>
#include <rendering/texture/texture.h>

#include "../ui/windows/insight_panel_window.h"

TextureAsset::TextureAsset() : meta(),
textureResource(nullptr)
{
}

TextureAsset::~TextureAsset()
{
}

void TextureAsset::onDefaultLoad(const FS::Path& metaPath)
{
	load(TextureType::IMAGE);
	AssetMeta::loadInto<Meta>(&meta, metaPath);
}

void TextureAsset::onUnload()
{
	if (!textureResource)
		return;

	ApplicationContext::resourceManager().release(textureResource->resourceId());
}

void TextureAsset::renderInspectableUI()
{
	if (!textureResource) {
		IMComponents::label("Error: Texture is not available", EditorUI::getFonts().h2_bold, IM_COL32(255, 100, 100, 255));
		return;
	}

	IMComponents::label(path().filename().string(), EditorUI::getFonts().h2_bold);
	ImGui::Separator();
	float contentX = ImGui::GetContentRegionAvail().x;
	ImGui::Image(textureResource->backendId(), ImVec2(contentX, contentX), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::Dummy(ImVec2(0.0f, 15.0f));
	IMComponents::label("GUID: " + std::to_string(id()), EditorUI::getFonts().h2_bold, IM_COL32(255, 100, 100, 255));
}

uint32_t TextureAsset::icon() const
{
	if (!textureResource)
		return IconPool::get("texture");

	return textureResource->backendId();
}

void TextureAsset::load(TextureType type)
{
	if (textureResource) {
		Console::out::warning("Texture Asset", "HOT RELOAD TEXTURE!");
		return; // Resource already created; later hot reload it here instead
	}

	std::string name = path().filename().string();

	auto& resource = ApplicationContext::resourceManager();
	auto [id, texture] = resource.create<Texture>(name);

	texture->setSource(type, path().string());
	resource.exec(texture->create());

	textureResource = texture;
}