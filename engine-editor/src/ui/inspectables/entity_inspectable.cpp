#include "entity_inspectable.h"

#include <glm.hpp>

#include "../src/ui/editor_ui.h"
#include "../src/core/ecs/components.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/search_popup/search_popup.h"
#include "../src/ui/windows/insight_panel_window.h"
#include "../src/editor_systems/component_registry.h"
#include "../src/ui/components/inspectable_components.h"

EntityInspectable::EntityInspectable(HierarchyItem& item) : item(item)
{
}

void EntityInspectable::renderStaticContent(ImDrawList& drawList)
{
	IMComponents::label(item.entity.name(), EditorUI::getFonts().h3_bold);
	ImGui::Dummy(ImVec2(0.0f, 3.0f));

	ImVec2 searchPosition = ImGui::GetCursorScreenPos() + ImVec2(0.0f, 38.0f);
	if (IMComponents::buttonBig("Add Component")) SearchPopup::searchComponents(searchPosition);
}

void EntityInspectable::renderDynamicContent(ImDrawList& drawList)
{
	const auto& components = ComponentRegistry::get();
	const auto& keysOrdered = ComponentRegistry::keysOrdered();

	for (const auto& key : keysOrdered) {
		components.at(key).tryDrawInspectable(item.entity.root);
	}
}