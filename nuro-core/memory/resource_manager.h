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
	void updateContext();

	// Queues the execution of a resource pipe for asynchronous execution
	bool exec(ResourcePipe&& pipe);

	// Executes a resource pipe synchronously, only possible until the first pipe was queued for asynchronous execution
	bool execAsDependency(ResourcePipe&& pipe);

	// Creates a new resource (lifetime managed by resource manager)
	template <typename T, typename... Args>
	std::pair<ResourceID, ResourceRef<T>> create(const std::string& name, Args&&... args) {
		static_assert(std::is_base_of<Resource, T>::value, "Only classes that derive from Resource are valid for allocation");

		// Create and return resource
		ResourceID id = ++idCounter;
		auto& result = resources.emplace(id, std::make_shared<T>(std::forward<Args>(args)...));
		auto& resource = result.first->second;
		resource->_resourceId = id;
		resource->_resourceName = name;
		return std::make_pair(id, std::static_pointer_cast<T>(resource));
	}

	// Retrieves an optional resource handle for a resource base by resource id
	OptResource<Resource> getResource(ResourceID id) {
		// Find resource
		auto it = resources.find(id);
		if (it != resources.end())
			return it->second;

		// Resource not found
		return std::nullopt;
	}

	// Retrieves an optional handle for a derived type T of resource by resource id
	template <typename T>
	OptResource<T> getResourceAs(ResourceID id) {
		static_assert(std::is_base_of<Resource, T>::value, "Only classes that derive from Resource are retrievable");

		// Find resource
		auto it = resources.find(id);
		if (it != resources.end())
			return std::static_pointer_cast<T>(it->second);

		// Resource not found
		return std::nullopt;
	}

	//
	// TEMPORARY!
	//

	struct ProcessorState {
		bool loading;
		std::string name;

		void setSleeping() { loading = false; name = ""; }
		void setLoading(std::string name) { loading = true; this->name = name; }
	};

	const ProcessorState& readProcessorState() {
		return processorState;
	}

	const auto& readResources() {
		return resources;
	}

private:
	// Counter for unique resource ids throughout application lifetime
	ResourceID idCounter;

	// Registry mapping a resource id to its resource reference
	std::unordered_map<ResourceID, ResourceRef<Resource>> resources;

	//
	// RESOURCE PIPE PROCESSING
	//

	// Resource pipes queued for async execution
	ConcurrentQueue<std::unique_ptr<ResourcePipe>> asyncPipes;

	// Processes pending async pipes
	void asyncPipeProcessor();

	std::atomic<bool> processorRunning;
	std::thread processor;
	std::mutex mtxProcessor;
	ProcessorState processorState;

	std::condition_variable cvNextPipe;
	std::condition_variable cvAwaitingContext;

	std::atomic<bool> contextNext;
	std::atomic<bool> contextResult;
	ResourceTask::TaskFunc contextTask;
};