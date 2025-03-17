#pragma once

#include <queue>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <utility>
#include <unordered_map>
#include <condition_variable>

#include <resource/resource.h>

class ResourceManager
{
public:
	struct WorkerState {
		bool active;
		Resource* target;
		size_t tasksPending;

		WorkerState(bool active, Resource* target, size_t tasksPending) : active(active), target(target), tasksPending(tasksPending) {};
	};

public:
	ResourceManager();
	~ResourceManager();

	// Creates a new resource (lifetime managed by resource manager), returns its resource id and pointer 
	template <typename T, typename... Args>
	std::pair<uint32_t, T*> create(Args&&... args) {
		return allocate<T>(std::forward<Args>(args)...);
	}

	// Returns the pointer to a resource by its id
	template <typename T>
	T* get(uint32_t id) {
		return retrieve<T>(id);
	}

	// Loads resource synchronously, blocking until complete  
	void loadSync(Resource* resource);

	// Queues resource for asynchronous loading, not blocking
	void loadAsync(Resource* resource);

	// Dispatch next pending resource to gpu (call when updating frame on main thread)
	void dispatchNext();

	// Returns the current state of the worker
	WorkerState readWorkerState() const;

	// TEMPORARY!
	const std::unordered_map<uint32_t, Resource*>& readResources() {
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
	std::unordered_map<uint32_t, Resource*> resources;

	template <typename T, typename... Args>
	std::pair<uint32_t, T*> allocate(Args&&... args) {
		static_assert(std::is_base_of<Resource, T>::value, "Only classes that derive from Resource are valid for allocation");
		
		// Create and return resource
		uint32_t id = ++idCounter;
		T* resource = new T(std::forward<Args>(args)...);
		resource->_resourceId = id;
		resources[id] = resource;
		return std::make_pair(id, resource);
	}

	template <typename T>
	T* retrieve(uint32_t id) {
		static_assert(std::is_base_of<Resource, T>::value, "Only classes that derive from Resource are retrievable");
		
		// Find resource
		auto it = resources.find(id);
		if (it != resources.end()) {
			return dynamic_cast<T*>(resources[id]);
		}

		// Resource not found, dynamically allocate empty resource
		Console::out::warning("Resource Manager", "A resource with an invalid id was accessed!");
		uint32_t id = allocate<T>();
		return resources[id];
	}

	//
	// MAIN THREAD
	//

	// Set if the application is running
	std::atomic<bool> running;

	// Resource upload tasks on the main thread
	std::queue<Resource*> mainTasks;

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

	// Resource load tasks on the worker thread
	std::queue<Resource*> workerTasks;

	// Amount of tasks waiting in workers queue
	std::atomic<size_t> workerTasksPending;

	// Set if the worker is currently running, false if worker is sleeping
	std::atomic<bool> workerActive;

	// Points to the resource the worker is currently loading (nullptr if worker isn't loading any resource)
	std::atomic<Resource*> workerTarget;
};