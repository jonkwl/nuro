#pragma once

#include <string>

#include "editor_asset.h"

#include <memory/resource_manager.h>
#include <rendering/texture/texture.h>

enum class TextureType;

class TextureAsset : public EditorAsset {
public:
	struct TextureSize {
		int width = 256;
		int height = 256;
	};

	struct Meta {
		TextureType type = TextureType::IMAGE;
		TextureSize size;
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