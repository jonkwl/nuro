#pragma once

#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <unordered_map>

#include "../core/ecs/ecs_collection.h"
#include "../src/core/rendering/gizmos/imgizmo.h"

namespace ComponentRegistry
{

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
    };

    // Creates global editor component registry
    void create();

    // Returns editor component registry
    const std::unordered_map<std::string, ComponentInfo>& get();

    // Returns ordered keys of editor component registry
    const std::vector<std::string>& keysOrdered();

};