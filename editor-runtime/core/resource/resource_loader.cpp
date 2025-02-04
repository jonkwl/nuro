#include "resource_loader.h"

#include <chrono>

#include "../core/utils/console.h"

ResourceLoader::ResourceLoader() : workerState()
{
	// Launch worker
	running = true;
	worker = std::jthread(&ResourceLoader::asyncWorker, this);
}

ResourceLoader::~ResourceLoader()
{
	// Stop worker
	running = false;
	worker.request_stop();
}

const ResourceLoader::WorkerState& ResourceLoader::readWorkerState() const
{
	return workerState;
}

void ResourceLoader::createSync(Resource* resource)
{
	resource->loadData();
	resource->dispatchGPU();
	resource->releaseData();
}

void ResourceLoader::createAsync(Resource* resource)
{
	// Add task to load tasks queue
	// Check: Possible race condition on the worker tasks queue
	workerTasks.push(resource);

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
		Resource* task = mainTasks.front();
		task->dispatchGPU();
		task->releaseData();
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

			// Update worker state
			workerState.workerActive = true;
			workerState.currentResource = resource;
			workerState.resourcesPending = workerTasks.size();

			// Load resource data
			resource->loadData();

			// Tmp for simulating larger resources
			std::this_thread::sleep_for(std::chrono::seconds(1));

			// Update queues
			popSafe(workerTasks);
			mainTasks.push(resource);

			mainDispatchNext = true;
			workerAwaitingDispatch.wait(lock, [&]() { return !mainDispatchNext; });
		}

		// Clear worker state
		workerState.workerActive = false;
		workerState.currentResource = nullptr;
		workerState.resourcesPending = 0;
	}
}