#pragma once

#include "editor_asset.h"

class TextureAsset : public EditorAsset {
public:
	TextureAsset();
	~TextureAsset() override;

	void inspect() const override;
	bool loading() const override;
	uint32_t icon() const override;

private:
	bool _loading;
};