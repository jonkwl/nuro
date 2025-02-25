#pragma once

#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <unordered_map>

#include <ecs/ecs_collection.h>
#include <rendering/gizmos/imgizmo.h>

// Reflection type for an editor compatible ecs component
struct ComponentInfo {

    // Name of component
    std::string name;

    // Editor component icon identifier
    std::string icon;

    // Function adding component to an entity
    std::function<void(Entity)> add;

    // Function drawing insight panel component inspectable for an entity if that entity has the component
    std::function<void(Entity)> tryDrawInspectable;

    // Function drawing component gizmo for an entity if component has gizmo and entity has the component
    std::function<void(Entity, IMGizmo&)> tryDrawGizmo;

    // Function drawing scene view icons for all instances of component if it has a scene view icon (taking in gizmo instance and transform of camera rendering)
    std::function<void(IMGizmo&, TransformComponent&)> tryDrawSceneIcons;
};

namespace ComponentRegistry
{

    // Creates global editor component registry
    void create();

    // Returns editor component registry
    const std::unordered_map<std::string, ComponentInfo>& get();

    // Returns ordered keys of editor component registry
    const std::vector<std::string>& keysOrdered();

};