#pragma once

#include <entt.hpp>

#include "../core/ecs/ecs.h"

class Entity
{
public:
	Entity();
	explicit Entity(entt::entity entity);

	static Entity create();
	void destroy();

	bool isValid();

	template<typename T, typename... Args>
	T& addComponent(Args&&... args) {
		return ECS::registry.emplace<T>(handle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent() {
		return ECS::registry.get<T>(handle);
	}

	template<typename T>
	bool hasComponent() {
		return ECS::registry.any_of<T>(handle);
	}

	template<typename T>
	void removeComponent() {
		ECS::registry.remove<T>(handle);
	}

private:
	entt::entity handle;
};

