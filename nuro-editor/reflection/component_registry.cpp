#include "component_registry.h"

#include <algorithm>
#include <cctype>

#include <utils/format.h>
#include <transform/transform.h>

#include "../gizmos/component_gizmos.h"
#include "../ui/components/inspectable_components.h"

namespace ComponentRegistry
{
	// Global registry for all ecs components
	std::unordered_map<std::string, ComponentInfo> gComponentRegistry;

    // Global ordered keys for registry
    std::vector<std::string> gKeysOrdered;

    // Registers a component onto the registry
    template<typename T>
    void registerComponent(
        const std::string& name,
        std::function<void(Entity, T&)> drawInspectable, 
        std::optional<std::function<void(IMGizmo&, TransformComponent&, T&)>> drawGizmo,
        bool hasSceneIcon
    ){
        std::string icon = Format::snakeCase(name);
        gComponentRegistry[name] = {
            // Name
            name,

            // Editor icon
            icon,

            // Has
            [](Entity entity) {
                return ECS::main().has<T>(entity);
            },

            // Add
            [](Entity entity) {
                ECS::main().add<T>(entity);
            },

            // Try draw inspectable
            [drawInspectable](Entity entity) {

                // Entity doesn't have component
                if (!ECS::main().has<T>(entity)) return;

                // Draw inspectable
                drawInspectable(entity, ECS::main().get<T>(entity));

            },

            // Try draw gizmo
            [drawGizmo](Entity entity, IMGizmo& gizmo) {
                
                // Component doesn't have custom gizmo
                if (!drawGizmo.has_value()) return;

                // Entity doesn't have component
                if (!ECS::main().has<T>(entity)) return;

                // Draw gizmo
                drawGizmo.value()(gizmo, ECS::main().get<TransformComponent>(entity), ECS::main().get<T>(entity));

            },

            // Try draw scene icons
            [hasSceneIcon, icon](IMGizmo& gizmos, TransformComponent& cameraTransform) {

                // Component doesn't have scene view icon
                if (!hasSceneIcon) return;

                // Scene view icon identifier
                std::string sceneIcon = icon + "_gizmo";

                // Render scene view icon for each component instance
                for (auto [entity, transform, component] : ECS::main().view<TransformComponent, T>().each()) {
                    gizmos.icon3d(IconPool::get(sceneIcon), Transform::getPosition(transform, Space::WORLD), cameraTransform);
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
            ComponentGizmos::drawPointLight,
            true
        );
        
        registerComponent<SpotlightComponent>(
            "Spotlight",
            InspectableComponents::drawSpotlight,
            ComponentGizmos::drawSpotlight,
            true
        );
        
        registerComponent<VelocityBlurComponent>(
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

        registerComponent<AudioListenerComponent>(
            "Audio Listener",
            InspectableComponents::drawAudioListener,
            std::nullopt,
            false
        );

        registerComponent<AudioSourceComponent>(
            "Audio Source",
            InspectableComponents::drawAudioSource,
            ComponentGizmos::drawAudioSource,
            true
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