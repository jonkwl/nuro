#pragma once

#include <queue>
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

template <typename T>
using ResourceRef = std::shared_ptr<T>;

template <typename T>
using OptResource = std::optional<ResourceRef<T>>;

class ResourceManager
{
public:
	struct WorkerState {
		bool active;
		uint32_t targetId;
		size_t tasksPending;

		WorkerState(bool active, uint32_t targetId, size_t tasksPending) : active(active), targetId(targetId), tasksPending(tasksPending) {};
	};

public:
	ResourceManager();
	~ResourceManager();

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

	// Loads resource synchronously, blocking until complete  
	void loadSync(uint32_t resourceId);

	// Queues resource for asynchronous loading, not blocking
	void loadAsync(uint32_t resourceId);

	// Destroy buffers (e.g. on the gpu) of a resource if it is ready
	void deleteBuffers(uint32_t resourceId);

	// Dispatch next pending resource to gpu (call when updating frame on main thread)
	void dispatchNext();

	// Returns the current state of the worker
	WorkerState readWorkerState() const;

	// TEMPORARY!
	const auto& readResources() {
		return resources;
	}

private:
	// Async worker thread
	void asyncWorker();

	// Helper to pop from queue safely
	template <typename T>
	void popSafe(std::queue<T>& queue) {
		if (!queue.empty()) queue.pop();
	}

private:
	//
	// RESOURCE ALLOCATION
	// 
	
	uint32_t idCounter;
	std::unordered_map<uint32_t, ResourceRef<Resource>> resources;

	//
	// MAIN THREAD
	//

	// Set if the application is running
	std::atomic<bool> running;

	// Resource upload tasks on the main thread by resource id
	std::queue<uint32_t> mainTasks;

	// Atomic set if main thread can dispatch next resource
	std::atomic<bool> mainDispatchNext;
	
	//
	// WORKER THREAD
	//

	// Worker thread handle
	std::thread worker;

	// Worker thread mutex
	std::mutex workerMtx;

	// Condition variable to ensure worker doesnt run when no tasks are available
	std::condition_variable workerTasksAvailable;

	// Condition variable to ensure worker doesnt run when a resource dispatch from the main thread is pending
	std::condition_variable workerAwaitingDispatch;

	// Resource load tasks on the worker thread by resource id
	std::queue<uint32_t> workerTasks;

	// Amount of tasks waiting in workers queue
	std::atomic<size_t> workerTasksPending;

	// Set if the worker is currently running, false if worker is sleeping
	std::atomic<bool> workerActive;

	// Id of the resource the worker is currently loading (0 if worker isn't loading any resource)
	std::atomic<uint32_t> workerTarget;

	//
	// HELPERS
	//

	// Frees the io data of a resource if it isn't set to be preserved
	void tryFreeIoData(ResourceRef<Resource>& resource);
};