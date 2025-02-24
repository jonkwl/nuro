#pragma once

#define G_REGISTRY ECS::gRegistry

#include <tuple>
#include <cctype>
#include <string>
#include <entt/entt.hpp>
#include <typeinfo>
#include <type_traits>

#include <ecs/ecs.h>
#include <utils/console.h>
#include <ecs/components.h>

///////////////////////////////////////////////////////////////////
///																///
///			SECURE CONTAINER FOR HANDLING ECS ENTITIES			///
///																///
///////////////////////////////////////////////////////////////////

class EntityContainer {
public:
	// Construct entity by root
	explicit EntityContainer(std::string name, Entity root) : root(root), 
	registry(G_REGISTRY), 
	transform(get<TransformComponent>()) 
	{
		transform.name = name;
	};

	// Construct entity container with data tuple
	explicit EntityContainer(std::string name, std::tuple<Entity, TransformComponent&> data) : root(std::get<0>(data)), 
	registry(G_REGISTRY),
	transform(std::get<1>(data)) 
	{
		transform.name = name;
	};

	// Entity containers backend entity handle
	Entity root;

	// Registry entity container is bound to
	Registry& registry;

	// Transform component of entity
	TransformComponent& transform;

	// Returns name of entity
	inline std::string name() {
		return transform.name;
	}

	// Returns if entity is valid in registry
	inline bool verify() {
		return registry.valid(root);
	}

	// Returns true if entity has a component of given component type
	template<typename T>
	inline bool has() {
		return registry.any_of<T>(root);
	}

	// Adds a component of given component type with given arguments to the entity
	template<typename T, typename... Args>
	T& add(Args&&... args) {

		// Fail if entity isn't valid
		if (!verify()) {
			verifyFailed();
			static T defaultComponent;
			return defaultComponent;
		}

		// Fail if entity already has component
		if (has<T>()) {
			operationFailed<T>("add", "already owns an instance of it");
			static T defaultComponent;
			return defaultComponent;
		}

		// Emplace component
		return registry.emplace<T>(root, std::forward<Args>(args)...);
	}

	// Returns component of given component type if attached to entity
	template<typename T>
	T& get() {

		// Fail if entity isn't valid
		if (!verify()) {
			verifyFailed();
			static T defaultComponent;
			return defaultComponent;
		}

		// Use EnTT's try_get to safely attempt to get the component
		auto component = registry.try_get<T>(root);
		if (!component) {
			operationFailed<T>("get", "doesn't own an instance of it");
			static T defaultComponent;
			return defaultComponent;
		}

		// Return component
		return *component;
	}

	// Removes component of given component type if attached to entity
	template<typename T>
	void remove() {

		// Fail if entity isn't valid
		if (!verify()) {
			verifyFailed();
			return;
		}

		// Use EnTT's has to check if the entity has the component
		if (!has<T>()) {
			operationFailed<T>("remove", "doesn't own an instance of it");
			return;
		}

		// Fail if component to remove is transform component
		if (std::is_same<T, TransformComponent>::value) {
			operationFailed<T>("remove", "must own a transform component");
			return;
		}

		// Erase component
		registry.erase<T>(root);
	}

	// Compares entity class with ecs root
	bool operator == (const Entity& other) const {
		return root == other;
	}

private:
	template<typename T>
	std::string getTypename() {
		// Get base typename
		std::string base = typeid(T).name();

		// Remove "struct " keyword if present (.name() is compiler dependant)
		size_t pos = base.find("struct ");
		if (pos != std::string::npos) base.erase(pos, 7);

		// Construct formatted typename
		std::string result;
		for (char c : base) {
			if (std::isupper(c) && !result.empty()) {
				result += ' ';
			}
			result += std::tolower(c);
		}

		return result;
	}

	template<typename T>
	void operationFailed(std::string operation, std::string reason) {
		Console::out::warning("Entity Container", "Couldn't " + operation + " " + getTypename<T>() + " because entity '" + name() + "' " + reason + ".");
	}

	void verifyFailed() {
		Console::out::warning("Entity Container", "Couldn't perform operation on entity '" + name() + "' because it doesn't exist.");
	}
};