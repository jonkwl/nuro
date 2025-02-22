#include "component_registry.h"

#include <algorithm>
#include <cctype>

#include "../src/gizmos/component_gizmos.h"
#include "../src/ui/components/inspectable_components.h"

namespace ComponentRegistry
{
	// Global registry for all ecs components
	std::unordered_map<std::string, ComponentInfo> gComponentRegistry;

    // Global ordered keys for registry
    std::vector<std::string> gKeysOrdered;

    // Converts given string to snake case
    std::string _snakeCase(std::string str) {
        for (char& c : str) {
            if (std::isspace(c)) {
                c = '_';
            }
            else {
                c = std::tolower(c);
            }
        }
        return str;
    }

    // Registers a component onto the registry
    template<typename T>
    void registerComponent(
        const std::string& name,
        std::function<void(Entity, T&)> drawInspectable, 
        std::optional<std::function<void(IMGizmo&, TransformComponent&, T&)>> drawGizmo,
        bool hasSceneIcon
    ){
        std::string icon = _snakeCase(name);
        gComponentRegistry[name] = {
            // Name
            name,

            // Editor icon
            icon,

            // Add
            [](Entity entity) {
                ECS::gRegistry.emplace<T>(entity);
            },

            // Try draw inspectable
            [drawInspectable](Entity entity) {

                // Entity doesn't have component
                if (!ECS::gRegistry.any_of<T>(entity)) return;

                // Draw inspectable
                drawInspectable(entity, ECS::gRegistry.get<T>(entity));

            },

            // Try draw gizmo
            [drawGizmo](Entity entity, IMGizmo& gizmo) {
                
                // Component doesn't have custom gizmo
                if (!drawGizmo.has_value()) return;

                // Entity doesn't have component
                if (!ECS::gRegistry.any_of<T>(entity)) return;

                // Draw gizmo
                drawGizmo.value()(gizmo, ECS::gRegistry.get<TransformComponent>(entity), ECS::gRegistry.get<T>(entity));

            },

            // Try draw scene icons
            [icon, hasSceneIcon](IMGizmo& gizmos, TransformComponent& cameraTransform) {

                // Component doesn't have scene view icon
                if (!hasSceneIcon) return;

                // Scene view icon identifier
                std::string sceneIcon = icon + "_gizmo";

                // Render scene view icon for each component instance
                for (auto [entity, transform, camera] : ECS::gRegistry.view<TransformComponent, T>().each()) {
                    gizmos.icon3d(IconPool::get(sceneIcon), transform.position, cameraTransform);
                }

            }
        };
        gKeysOrdered.push_back(name);
    }

	void create()
	{
        registerComponent<TransformComponent>(
            "Transform",
            InspectableComponents::drawTransform,
            std::nullopt,
            false
        );
        
        registerComponent<MeshRendererComponent>(
            "Mesh Renderer",
            InspectableComponents::drawMeshRenderer,
            std::nullopt,
            false
        );
        
        registerComponent<CameraComponent>(
            "Camera",
            InspectableComponents::drawCamera,
            ComponentGizmos::drawCamera,
            true
        );
        
        registerComponent<DirectionalLightComponent>(
            "Directional Light", 
            InspectableComponents::drawDirectionalLight,
            std::nullopt,
            true
        );
        
        registerComponent<PointLightComponent>(
            "Point Light",
            InspectableComponents::drawPointLight,
            std::nullopt,
            true
        );
        
        registerComponent<SpotlightComponent>(
            "Spotlight",
            InspectableComponents::drawSpotlight,
            std::nullopt,
            true
        );
        
        registerComponent<VelocityComponent>(
            "Velocity Blur", 
            InspectableComponents::drawVelocity,
            std::nullopt,
            false
        );
        
        registerComponent<BoxColliderComponent>(
            "Box Collider",
            InspectableComponents::drawBoxCollider,
            ComponentGizmos::drawBoxCollider,
            false
        );
        
        registerComponent<SphereColliderComponent>(
            "Sphere Collider",
            InspectableComponents::drawSphereCollider,
            ComponentGizmos::drawSphereCollider,
            false
        );
        
        registerComponent<RigidbodyComponent>(
            "Rigidbody",
            InspectableComponents::drawRigidbody,
            std::nullopt,
            false
        );
	}

    const std::unordered_map<std::string, ComponentInfo>& get()
    {
        return gComponentRegistry;
    }

    const std::vector<std::string>& keysOrdered()
    {
        return gKeysOrdered;
    }

};