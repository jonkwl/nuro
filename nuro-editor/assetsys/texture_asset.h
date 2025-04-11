#pragma once

#include "editor_asset.h"

enum class TextureType;

class TextureAsset : public EditorAsset {
public:
	TextureAsset();
	~TextureAsset() override;

	void onDefaultLoad() override;
	void onUnload() override;

	uint32_t icon() const override;

private:
	void load(TextureType type);
};