#include "resource_loader.h"

#include <chrono>

#include "../core/utils/console.h"

ResourceLoader::ResourceLoader()
{
	// Launch worker
	running = true;
	worker = std::thread(&ResourceLoader::asyncWorker, this);
}

ResourceLoader::~ResourceLoader()
{
	// Stop worker
	running = false;
	worker.join();
}

ResourceLoader::WorkerState ResourceLoader::readWorkerState() const
{
	return WorkerState(workerActive, workerTarget, workerTasksPending);
}

void ResourceLoader::createSync(Resource* resource)
{
	// Load resources data
	resource->loadData();

	// Dispatch resource to gpu
	resource->dispatchGPU();

	// Release resources data
	resource->releaseData();
}

void ResourceLoader::createAsync(Resource* resource)
{
	// Add task to load tasks queue
	workerTasks.push(resource); // Possible race condition on the worker tasks queue

	// Update resources state
	resource->state = ResourceState::QUEUED;

	// Update worker state
	workerTasksPending++;

	// Notify worker
	workerTasksAvailable.notify_one();
}

void ResourceLoader::dispatchNext()
{
	// [MAIN THREAD]

	// Only dispatch next resource if allowed by worker before
	if (!mainDispatchNext) return;

	// Make sure worker thread is locked during dispatch (since main tasks is a shared resource)
	std::lock_guard<std::mutex> lock(workerMtx);
	
	// Dispatch next resource in queue (only one per frame to prevent heavy main thread blocking)
	if (!mainTasks.empty()) {
		// Fetch resource to be dispatched
		Resource* resource = mainTasks.front();

		// Dispatch resource and release its data
		resource->dispatchGPU();
		resource->releaseData();

		// Update resources state
		resource->state = ResourceState::READY;

		// Pop resource from queue safely
		popSafe(mainTasks);
	}

	// Resource dispatched, prevent another dispatch
	mainDispatchNext = false;

	// Unlock worker mutex and continue worker thread
	workerAwaitingDispatch.notify_one();
}

void ResourceLoader::asyncWorker()
{
	// [WORKER THREAD]

	while (running) {

		std::unique_lock<std::mutex> lock(workerMtx);

		// Condition: Queue with tasks to load must not be empty
		if (workerTasks.empty()) {
			workerTasksAvailable.wait(lock, [&]() { return !workerTasks.empty(); });
		}

		while (!workerTasks.empty()) {
			// Fetch resource to be loaded
			Resource* resource = workerTasks.front();

			// Update resources state
			resource->state = ResourceState::CREATING;

			// Sync worker state
			workerTasksPending = workerTasks.size();
			workerActive = true;
			workerTarget = resource;

			// Load resource data
			resource->loadData();

			// Update queues
			popSafe(workerTasks);
			mainTasks.push(resource);

			mainDispatchNext = true;
			workerAwaitingDispatch.wait(lock, [&]() { return !mainDispatchNext; });
		}

		// Sync worker state
		workerTasksPending = 0;
		workerActive = false;
		workerTarget = nullptr;
	}
}