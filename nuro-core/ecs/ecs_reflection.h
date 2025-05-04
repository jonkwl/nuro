#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

namespace ECSReflection {

	// Registers all types and their associated reflection metadata
	void registerAll();

	// Serializes an ecs component instance to json
	json serializeComponent(entt::meta_any instance);

	// Deserializes a json component emplacing it onto the given entity
	bool deserializeComponent(entt::entity entity, const json& source);

}