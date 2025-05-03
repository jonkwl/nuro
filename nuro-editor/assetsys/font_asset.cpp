#include "font_asset.h"

#include <imgui.h>

#include <rendering/icons/icon_pool.h>

#include "../ui/components/im_components.h"
#include "../ui/windows/insight_panel_window.h"

FontAsset::FontAsset() : meta(),
handle(nullptr)
{
}

FontAsset::~FontAsset()
{
}

void FontAsset::onDefaultLoad(const FS::Path& metaPath)
{
	AssetMeta::loadMeta<Meta>(&meta, metaPath);
}

void FontAsset::onUnload()
{
	//
}

void FontAsset::onReload()
{
	//
}

void FontAsset::renderInspectableUI()
{
	if (!handle) {
		IMComponents::label("Font is not loaded", EditorUI::getFonts().h3, IM_COL32(255, 255, 255, 255));

		if (IMComponents::buttonBig("Load Font (Permanent)"))
			load();

		return;
	}

	std::string sampleText = "The quick brown fox jumps over the lazy dog.";
	IMComponents::label(sampleText, handle, IM_COL32(255, 255, 255, 255));
}

uint32_t FontAsset::icon() const
{
	return IconPool::get("font");
}

ImFont* FontAsset::load()
{
	FS::Path fontPath = path();
	if (!FS::hasExtension(fontPath, ".ttf") && !FS::hasExtension(fontPath, ".otf"))
		return nullptr;

	// Load font here later (can't happen while rendering ui)
}