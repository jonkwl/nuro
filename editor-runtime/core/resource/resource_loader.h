#pragma once

#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../core/resource/resource.h"

class ResourceLoader
{
public:
	struct WorkerState {
		// Set if the worker is currently loading resources, false if worker is sleeping
		bool workerActive = false;

		// Points to the resource the worker is loading (nullptr if worker isn't working)
		Resource* currentResource = nullptr;

		// Amount of resources waiting in queue to be loaded
		uint32_t resourcesPending = 0;
	};

public:
	ResourceLoader();
	~ResourceLoader();

	// Returns the read-only state of the worker
	const WorkerState& readWorkerState() const;

	// Creates resources synchronously, blocking until complete  
	void createSync(Resource* resource);

	// Queues resource creation for asynchronous creation, not blocking
	void createAsync(Resource* resource);

	// Dispatch next pending resource to gpu (on main thread)
	void dispatchNext();

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
	// STATE
	//

	std::atomic<bool> running;

	//
	// MAIN THREAD
	//

	// Resource upload tasks on the main thread
	std::queue<Resource*> mainTasks;

	// Atomic set if main thread can dispatch next resource
	std::atomic<bool> mainDispatchNext;
	
	//
	// WORKER THREAD
	//

	std::jthread worker;
	std::mutex workerMtx;
	std::condition_variable workerTasksAvailable;
	std::condition_variable workerAwaitingDispatch;

	// Resource load tasks on the worker thread
	std::queue<Resource*> workerTasks;

	// State of the worker, only to be changed by worker thread
	WorkerState workerState;
};