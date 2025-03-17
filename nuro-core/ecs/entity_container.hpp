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

///////////////////////////////////////////////////////////////////
///																///
///			SECURE CONTAINER FOR HANDLING ECS ENTITIES			///
///																///
///////////////////////////////////////////////////////////////////

class EntityContainer {
public:
	// Construct empty entity container
	EntityContainer() : _handle(entt::null), _transform(nullptr)
	{
	}

	// Copy entity container
	EntityContainer(const EntityContainer& other) : _handle(other._handle), _transform(other._transform)
	{
	}

	// Construct entity container by entity handle
	explicit EntityContainer(Entity handle) : _handle(handle), _transform(&get<TransformComponent>())
	{
	}

	// Construct entity container by ecs native entity creation data tuple
	explicit EntityContainer(std::tuple<Entity, TransformComponent&> data) : _handle(std::get<0>(data)), _transform(&std::get<1>(data))
	{
	}

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
		return *_transform;
	}

	// Returns id of entity
	inline uint32_t id() const {
		if (!_transform) return 0;
		return _transform->id;
	}

	// Returns name of entity
	inline std::string name() const {
		if (!_transform) return "Invalid Entity";
		return _transform->name;
	}

	// Returns if entity is valid in registry
	inline bool verify() const {
		return ECS::main().verify(_handle);
	}

	// Returns true if entity has a component of given component type
	template<typename T>
	inline bool has() const {
		return ECS::main().has<T>(_handle);
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

		// Fail if component cant be added
		if (!ECS::main().canAdd<T>(_handle)) {
			componentOperationFailed<T>("add", "already owns an instance of it");
			static T defaultComponent;
			return defaultComponent;
		}

		// Emplace component
		return ECS::main().add<T>(_handle, std::forward<Args>(args)...);
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

		// Make sure entity has component
		if (!has<T>()) {
			componentOperationFailed<T>("get", "doesn't own an instance of it");
			static T defaultComponent;
			return defaultComponent;
		}

		// Fetch and return component
		return ECS::main().get<T>(_handle);
	}

	// Removes component of given component type if attached to entity
	template<typename T>
	void remove() {
		ECS::main().remove<T>(_handle);
	}

private:
	// Entity containers backend entity handle
	Entity _handle;

	// Transform component of entity
	TransformComponent* _transform;

private:
	template<typename T>
	void componentOperationFailed(std::string operation, std::string reason) {
		Console::out::warning("Entity Container", "Couldn't " + operation + " some component because entity '" + name() + "' " + reason + "");
	}

	void fetchTransformFailed() {
		Console::out::warning("Entity Container", "Couldn't fetch transform component of some entity");
	}

	void verifyFailed() {
		Console::out::warning("Entity Container", "Couldn't perform operation on some entity because it doesn't exist");
	}
};