#include "entity_inspectable.h"

#include <glm.hpp>

#include "../src/core/ecs/components.h"

#include "../src/ui/editor_ui.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/search_popup/search_popup.h"
#include "../src/ui/windows/insight_panel_window.h"
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
	component<TransformComponent>(InspectableComponents::drawTransform);
	component<MeshRendererComponent>(InspectableComponents::drawMeshRenderer);
	component<CameraComponent>(InspectableComponents::drawCamera);
	component<DirectionalLightComponent>(InspectableComponents::drawDirectionalLight);
	component<PointLightComponent>(InspectableComponents::drawPointLight);
	component<SpotlightComponent>(InspectableComponents::drawSpotlight);
	component<VelocityComponent>(InspectableComponents::drawVelocity);
	component<BoxColliderComponent>(InspectableComponents::drawBoxCollider);
	component<SphereColliderComponent>(InspectableComponents::drawSphereCollider);
	component<RigidbodyComponent>(InspectableComponents::drawRigidbody);
}