#include "texture_asset.h"

#include <rendering/icons/icon_pool.h>

#include "../ui/windows/insight_panel_window.h"
#include "../ui/inspectables/texture_inspectable.h"

TextureAsset::TextureAsset() : _loading(false)
{
}

TextureAsset::~TextureAsset()
{
}

void TextureAsset::inspect() const
{
	InsightPanelWindow::inspect<TextureInspectable>();
}

bool TextureAsset::loading() const
{
	return _loading;
}

uint32_t TextureAsset::icon() const
{
	return IconPool::get("texture");
}