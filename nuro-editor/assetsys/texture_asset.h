#pragma once

#include <string>

#include "editor_asset.h"

#include <memory/resource_manager.h>
#include <rendering/texture/texture.h>

enum class TextureType;

class TextureAsset : public EditorAsset {
public:
	struct Meta {
		int size = 256;
	};

	TextureAsset();
	~TextureAsset() override;

	void onDefaultLoad(const FS::Path& metaPath) override;
	void onUnload() override;

	void renderInspectableUI() override;

	uint32_t icon() const override;

private:
	Meta meta;
	ResourceRef<Texture> textureResource;

	void load(TextureType type);
};