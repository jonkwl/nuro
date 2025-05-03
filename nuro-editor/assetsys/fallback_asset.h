#pragma once

#include "editor_asset.h"

class FallbackAsset : public EditorAsset {
public:
	struct Meta {
		bool empty = true;
	};

	FallbackAsset();
	~FallbackAsset() override;

	void onDefaultLoad(const FS::Path& metaPath) override;
	void onUnload() override;
	void onReload() override;

	void renderInspectableUI() override;

	uint32_t icon() const override;

private:
	Meta meta;
};