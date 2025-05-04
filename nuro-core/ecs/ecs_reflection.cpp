#include "ecs_reflection.h"

#include <string>
#include <functional>

#include <glm/glm.hpp>

#include <ecs/ecs.h>
#include <utils/console.h>
#include <ecs/components.h>

#define START_CAST() \
	auto anyval = member.get(instance); \
	if (!anyval) continue;

#define NEXT_CAST(type) \
    else if (memberType == entt::resolve<type>()) { \
        auto value = anyval.cast<type>(); \
        properties[key] = value; \
    }

using namespace entt::literals;

struct Example {
	float x;
	float y;
};

namespace nlohmann {
	template <>
	struct adl_serializer<glm::vec3> {
		static void to_json(ordered_json& j, const glm::vec3& v) {
			j = json{
				{"x", v.x},
				{"y", v.y},
				{"z", v.z}
			};
		}

		static void from_json(const ordered_json& j, glm::vec3& v) {
			v.x = j.at("x").get<float>();
			v.y = j.at("y").get<float>();
			v.z = j.at("z").get<float>();
		}
	};

	template <>
	struct adl_serializer<glm::quat> {
		static void to_json(ordered_json& j, const glm::quat& q) {
			j = json{
				{"x", q.x},
				{"y", q.y},
				{"z", q.z},
				{"w", q.w}
			};
		}

		static void from_json(const ordered_json& j, glm::quat& q) {
			q.x = j.at("x").get<float>();
			q.y = j.at("y").get<float>();
			q.z = j.at("z").get<float>();
			q.w = j.at("w").get<float>();
		}
	};
}

namespace ECSReflection {

	template <typename T>
	entt::meta_any emplace(entt::entity entity) {
		T& component = ECS::main().reg().emplace_or_replace<T>(entity);
		return std::ref(component);
	}

	void registerAll() {
		entt::meta<TransformComponent>()
			.type("Transform"_hs)
			.func<&emplace<TransformComponent>>("emplace"_hs)
			.data<&TransformComponent::id>("id"_hs)
			.data<&TransformComponent::name>("name"_hs)
			.data<&TransformComponent::position>("position"_hs)
			.data<&TransformComponent::rotation>("rotation"_hs)
			.data<&TransformComponent::scale>("scale"_hs);

		entt::meta<MeshRendererComponent>()
			.type("MeshRenderer"_hs)
			.func<&emplace<MeshRendererComponent>>("emplace"_hs)
			.data<&MeshRendererComponent::enabled>("enabled"_hs);

		entt::meta<PointLightComponent>()
			.type("PointLight"_hs)
			.func<&emplace<PointLightComponent>>("emplace"_hs)
			.data<&PointLightComponent::enabled>("enabled"_hs)
			.data<&PointLightComponent::intensity>("intensity"_hs)
			.data<&PointLightComponent::color>("color"_hs)
			.data<&PointLightComponent::range>("range"_hs)
			.data<&PointLightComponent::falloff>("falloff"_hs);
	}

	json serializeComponent(entt::meta_any instance) {
		// Fetch type
		entt::meta_type type = instance.type();
		if (!type)
			return json();

		// Create json buffers
		json component{};
		json properties{};

		// Serialize each data member
		for (auto [id, member] : type.data()) {
			std::string key = std::to_string(id);
			auto memberType = member.type();
			if (!memberType)
				continue;
			
			try {
				START_CAST()
					NEXT_CAST(int)
					NEXT_CAST(float)
					NEXT_CAST(double)
					NEXT_CAST(bool)
					NEXT_CAST(std::string)
					NEXT_CAST(glm::vec3)
					NEXT_CAST(glm::quat)
			}
			catch (const std::exception& e) {
				Console::out::warning("ECS Reflection", "Error while serializing ECS component " + std::string(type.info().name()) + ": ", e.what());
				return json();
			}
			catch (...) {
				Console::out::warning("ECS Reflection", "Error while serializing ECS component " + std::string(type.info().name()));
				return json();
			}
		}

		// Set type and properties
		component["type"] = type.id();
		component["properties"] = properties;

		return component;
	}

	bool deserializeComponent(entt::entity entity, const json& source)
	{
		// Validate source format
		if (!source.contains("type") || !source.contains("properties"))
			return false;

		// Resolve component type
		auto xtype = source["type"];
		if (!xtype.is_number_unsigned())
			return false;
		auto typehash = xtype.get<entt::id_type>();
		entt::meta_type type = entt::resolve(typehash);
		if (!type)
			return false;

		// Fetch emplace function
		entt::meta_any caller = type.construct();
		entt::meta_func emplace = type.func("emplace"_hs);
		if (!emplace)
			return false;

		// Instantiate component
		entt::meta_any instance = emplace.invoke(caller, entity);
		entt::meta_type instanceType = instance.type();
		if (!instanceType)
			return false;

		// Set each data member of component
		for (auto [id, member] : instanceType.data()) {
			auto memberType = member.type();
			if (!memberType)
				continue;

			// Set values of each member
			// ...
		}

		return true;
	}

}