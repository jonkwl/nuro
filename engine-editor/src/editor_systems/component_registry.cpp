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
        std::optional<std::function<void(IMGizmo&, TransformComponent&, T&)>> drawGizmo)
    {
        std::string snakeName = _snakeCase(name);
        gComponentRegistry[name] = {
            // Name
            name,

            // Editor icon
            snakeName,

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

            }
        };
        gKeysOrdered.push_back(name);
    }

	void create()
	{
        registerComponent<TransformComponent>(
            "Transform",
            InspectableComponents::drawTransform,
            std::nullopt
        );
        
        registerComponent<MeshRendererComponent>(
            "Mesh Renderer",
            InspectableComponents::drawMeshRenderer,
            std::nullopt
        );
        
        registerComponent<CameraComponent>(
            "Camera",
            InspectableComponents::drawCamera,
            std::nullopt
        );
        
        registerComponent<DirectionalLightComponent>(
            "Directional Light", 
            InspectableComponents::drawDirectionalLight,
            std::nullopt
        );
        
        registerComponent<PointLightComponent>(
            "Point Light",
            InspectableComponents::drawPointLight,
            std::nullopt
        );
        
        registerComponent<SpotlightComponent>(
            "Spotlight",
            InspectableComponents::drawSpotlight,
            std::nullopt
        );
        
        registerComponent<VelocityComponent>(
            "Velocity Blur", 
            InspectableComponents::drawVelocity,
            std::nullopt
        );
        
        registerComponent<BoxColliderComponent>(
            "Box Collider",
            InspectableComponents::drawBoxCollider,
            ComponentGizmos::drawBoxCollider
        );
        
        registerComponent<SphereColliderComponent>(
            "Sphere Collider",
            InspectableComponents::drawSphereCollider,
            ComponentGizmos::drawSphereCollider
        );
        
        registerComponent<RigidbodyComponent>(
            "Rigidbody",
            InspectableComponents::drawRigidbody,
            std::nullopt
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