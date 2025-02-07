#include "entity_inspectable.h"

#include <glm.hpp>

#include "../core/ecs/components.h"

#include "../src/ui/editor_ui.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/windows/insight_panel_window.h"
#include "../src/ui/components/inspectable_components.h"

EntityInspectable::EntityInspectable(HierarchyItem& item) : item(item)
{
}

std::string EntityInspectable::getName()
{
	return item.entity.name;
}

void EntityInspectable::renderStaticContent(ImDrawList& drawList)
{
	IMComponents::label(item.entity.name, EditorUI::getFonts().h3_bold);
	ImGui::Dummy(ImVec2(0.0f, 3.0f));
	IMComponents::buttonBig("Add Component");
}

void EntityInspectable::renderDynamicContent(ImDrawList& drawList)
{
	add<TransformComponent>(InspectableComponents::drawTransform);
	add<MeshRendererComponent>(InspectableComponents::drawMeshRenderer);
	add<CameraComponent>(InspectableComponents::drawCamera);
	add<DirectionalLightComponent>(InspectableComponents::drawDirectionalLight);
	add<PointLightComponent>(InspectableComponents::drawPointLight);
	add<SpotlightComponent>(InspectableComponents::drawSpotlight);
	add<VelocityComponent>(InspectableComponents::drawVelocity);
	add<BoxColliderComponent>(InspectableComponents::drawBoxCollider);
	add<SphereColliderComponent>(InspectableComponents::drawSphereCollider);
	add<RigidbodyComponent>(InspectableComponents::drawRigidbody);
}