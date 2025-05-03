#include "fallback_asset.h"

#include <rendering/icons/icon_pool.h>

#include "../ui/windows/insight_panel_window.h"

FallbackAsset::FallbackAsset() : meta()
{
}

FallbackAsset::~FallbackAsset()
{
}

void FallbackAsset::onDefaultLoad(const FS::Path& metaPath)
{
	AssetMeta::loadMeta<Meta>(&meta, metaPath);
}

void FallbackAsset::onUnload()
{
	// Nothing to unload
}

void FallbackAsset::onReload()
{
	// Nothing to reload
}

void FallbackAsset::renderInspectableUI()
{
	IMComponents::label("Asset: " + path().filename().string(), EditorUI::getFonts().h3_bold);
}

uint32_t FallbackAsset::icon() const
{
	return IconPool::get("file");
}