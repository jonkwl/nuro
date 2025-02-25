#include "entity_inspectable.h"

#include <glm/glm.hpp>

#include "../ui/editor_ui.h"
#include <ecs/components.h>
#include "../ui/windows/registry_window.h"
#include "../ui/components/im_components.h"
#include "../ui/search/search_popup.h"
#include "../ui/windows/insight_panel_window.h"
#include "../reflection/component_registry.h"
#include "../ui/components/inspectable_components.h"

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