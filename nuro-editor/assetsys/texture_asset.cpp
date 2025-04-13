#include "texture_asset.h"

#include <rendering/icons/icon_pool.h>
#include <rendering/texture/texture.h>

#include "../ui/windows/insight_panel_window.h"
#include "../ui/inspectables/texture_inspectable.h"

TextureAsset::TextureAsset()
{
}

TextureAsset::~TextureAsset()
{
}

void TextureAsset::onDefaultLoad()
{
	load(TextureType::IMAGE_RGBA);
}

void TextureAsset::onUnload()
{
	if (_assetResource)
		ApplicationContext::resourceManager().release(_assetResource->resourceId());
}

uint32_t TextureAsset::icon() const
{
	if (loading())
		return IconPool::get("texture");

	if (_assetResource->resourceState() == ResourceState::FAILED)
		return IconPool::get("failed");

	if (auto texture = std::dynamic_pointer_cast<Texture>(_assetResource))
		return texture->backendId();
}

void TextureAsset::load(TextureType type)
{
	if (_assetResource)
		return; // Resource already created; later hot reload it here instead

	std::string name = path().filename().string();

	auto& resource = ApplicationContext::resourceManager();
	auto& [id, texture] = resource.create<Texture>(name);

	texture->setSource(type, path().string());
	resource.exec(texture->create());

	_assetResource = texture;
}