#include "asset_inspectable.h"

#include "../../runtime/runtime.h"

AssetInspectable::AssetInspectable(AssetSID assetId) : assetId(assetId)
{
}

void AssetInspectable::renderStaticContent(ImDrawList& drawList)
{
	if (auto asset = Runtime::projectManager().assets().get(assetId))
		asset->renderInspectableUI();
}

void AssetInspectable::renderDynamicContent(ImDrawList& drawList)
{
}