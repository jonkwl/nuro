#pragma once

#include <tuple>
#include <cctype>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <entt/entt.hpp>

#include <ecs/ecs.h>
#include <utils/console.h>
#include <ecs/components.h>

#define G_REGISTRY ECS::gRegistry

///////////////////////////////////////////////////////////////////
///																///
///			SECURE CONTAINER FOR HANDLING ECS ENTITIES			///
///																///
///////////////////////////////////////////////////////////////////

class EntityContainer {
public:
	// Construct empty entity container
	EntityContainer() : _registry(G_REGISTRY), _handle(entt::null), _transform(nullptr)
	{
	};

	// Copy entity container
	EntityContainer(const EntityContainer& other) : _registry(G_REGISTRY), _handle(other._handle), _transform(other._transform)
	{
	}

	// Construct entity container by entity handle
	explicit EntityContainer(Entity handle) : _registry(G_REGISTRY), _handle(handle), _transform(&get<TransformComponent>())
	{
	};

	// Construct entity container by ecs native entity creation data tuple
	explicit EntityContainer(std::tuple<Entity, TransformComponent&> data) : _registry(G_REGISTRY), _handle(std::get<0>(data)), _transform(&std::get<1>(data))
	{
	};

	// Assign entity container
	EntityContainer& operator=(const EntityContainer& other)
	{
		if (this != &other) {
			_handle = other._handle;
			_transform = other._transform;
		}
		return *this;
	}

	// Returns native entity handle
	inline Entity handle() {
		return _handle;
	}

	// Returns transform of entity
	inline TransformComponent& transform() {
		// Make sure transform is available
		if (!_transform) {
			fetchTransformFailed();
			static TransformComponent defaultTransform;
			return defaultTransform;
		}

		// Return transform as reference
		return *_transform;;
	}

	// Returns name of entity
	inline std::string name() {
		if (!_transform) return "Invalid Entity";
		return _transform->name;
	}

	// Returns if entity is valid in registry
	inline bool verify() {
		return _registry.valid(_handle);
	}

	// Returns true if entity has a component of given component type
	template<typename T>
	inline bool has() {
		return _registry.any_of<T>(_handle);
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
			componentOperationFailed<T>("add", "already owns an instance of it");
			static T defaultComponent;
			return defaultComponent;
		}

		// Emplace component
		return _registry.emplace<T>(_handle, std::forward<Args>(args)...);
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

		// Safely attempt to fetch the component
		auto component = _registry.try_get<T>(_handle);

		// Check if component was fetched
		if (!component) {
			componentOperationFailed<T>("get", "doesn't own an instance of it");
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

		// Check if entity has the component
		if (!has<T>()) {
			componentOperationFailed<T>("remove", "doesn't own an instance of it");
			return;
		}

		// Fail if component to remove is transform component
		if (std::is_same<T, TransformComponent>::value) {
			componentOperationFailed<T>("remove", "must own a transform component");
			return;
		}

		// Erase component
		_registry.erase<T>(_handle);
	}

private:
	// Registry entity container is bound to
	Registry& _registry;

	// Entity containers backend entity handle
	Entity _handle;

	// Transform component of entity
	TransformComponent* _transform;

private:
	template<typename T>
	void componentOperationFailed(std::string operation, std::string reason) {
		Console::out::warning("Entity Container", "Couldn't " + operation + " some component because entity '" + name() + "' " + reason + ".");
	}

	void fetchTransformFailed() {
		Console::out::warning("Entity Container", "Couldn't fetch transform component of some entity.");
	}

	void verifyFailed() {
		Console::out::warning("Entity Container", "Couldn't perform operation on some entity because it doesn't exist.");
	}
};