#pragma once

#include <string>

#include "editor_asset.h"

#include <utils/fsutil.h>

#include "../ui/editor_ui.h"

class ImFont;

class FontAsset : public EditorAsset {
public:
	struct Meta {
		uint32_t size = EditorSizing::h2_FontSize;
	};

	FontAsset();
	~FontAsset() override;

	void onDefaultLoad(const FS::Path& metaPath) override;
	void onUnload() override;
	void onReload() override;

	void renderInspectableUI() override;

	uint32_t icon() const override;

private:
	Meta meta;
	ImFont* handle;

	ImFont* load();
};