#pragma once

#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <utility>
#include <optional>
#include <unordered_map>
#include <condition_variable>

#include <utils/console.h>
#include <memory/resource.h>
#include <memory/resource_pipe.h>
#include <utils/concurrent_queue.h>

template <typename T>
using ResourceRef = std::shared_ptr<T>;

template <typename T>
using OptResource = std::optional<ResourceRef<T>>;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	// Updates the resource manager from the context thread
	void update();

	// Executes a resource pipe synchronously
	bool execSync(ResourcePipe pipe);

	// Queues the execution of a resource pipe for asynchronous execution
	bool execAsync(ResourcePipe pipe);

	// Creates a new resource (lifetime managed by resource manager)
	template <typename T, typename... Args>
	std::pair<uint32_t, ResourceRef<T>> create(const std::string& name, Args&&... args) {
		static_assert(std::is_base_of<Resource, T>::value, "Only classes that derive from Resource are valid for allocation");

		// Create and return resource
		uint32_t id = ++idCounter;
		auto& result = resources.emplace(id, std::make_shared<T>(std::forward<Args>(args)...));
		auto& resource = result.first->second;
		resource->_resourceId = id;
		resource->_resourceName = name;
		return std::make_pair(id, std::static_pointer_cast<T>(resource));
	}

	// Retrieves an optional resource handle for a resource base by resource id
	OptResource<Resource> getResource(uint32_t resourceId) {
		// Find resource
		auto it = resources.find(resourceId);
		if (it != resources.end())
			return it->second;

		// Resource not found
		return std::nullopt;
	}

	// Retrieves an optional handle for a derived type T of resource by resource id
	template <typename T>
	OptResource<T> getResourceAs(uint32_t resourceId) {
		static_assert(std::is_base_of<Resource, T>::value, "Only classes that derive from Resource are retrievable");

		// Find resource
		auto it = resources.find(resourceId);
		if (it != resources.end())
			return std::static_pointer_cast<T>(it->second);

		// Resource not found
		return std::nullopt;
	}

	// TEMPORARY!
	const auto& readResources() {
		return resources;
	}

private:
	uint32_t idCounter;
	std::unordered_map<uint32_t, ResourceRef<Resource>> resources;
};