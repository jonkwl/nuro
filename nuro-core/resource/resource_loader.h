#pragma once

#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <resource/resource.h>

class ResourceLoader
{
public:
	struct WorkerState {
		bool active;
		Resource* target;
		size_t tasksPending;

		WorkerState(bool active, Resource* target, size_t tasksPending) : active(active), target(target), tasksPending(tasksPending) {};
	};

public:
	ResourceLoader();
	~ResourceLoader();

	// Returns the current state of the worker
	WorkerState readWorkerState() const;

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
	//
	// MAIN THREAD
	//
	//

	// Set if the application is running
	std::atomic<bool> running;

	// Resource upload tasks on the main thread
	std::queue<Resource*> mainTasks;

	// Atomic set if main thread can dispatch next resource
	std::atomic<bool> mainDispatchNext;
	
	//
	//
	// WORKER THREAD
	//
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