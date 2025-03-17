#include "resource_manager.h"

#include <chrono>

ResourceManager::ResourceManager() : idCounter(0)
{
	// Set running
	running = true;

	// Launch worker
	worker = std::thread(&ResourceManager::asyncWorker, this);
}

ResourceManager::~ResourceManager()
{
	// Unset running
	running = false;

	// Detach worker if required
	if (worker.joinable()) worker.detach();

	// Delete resources
	for (auto& [id, resource] : resources) {
		resource->releaseData();
		delete resource;
	}
}

void ResourceManager::loadSync(Resource* resource)
{
	// Load resources data
	if (!resource->loadData()) {
		resource->_resourceState = ResourceState::FAILED;
		return;
	}

	// Dispatch resource to gpu
	if (!resource->dispatchGPU()) {
		resource->_resourceState = ResourceState::FAILED;
		resource->releaseData();
		return;
	}

	// Release resources data
	resource->releaseData();
}

void ResourceManager::loadAsync(Resource* resource)
{
	// Add task to load tasks queue
	workerTasks.push(resource); // Possible race condition on the worker tasks queue

	// Update resources state
	resource->_resourceState = ResourceState::QUEUED;

	// Update worker state
	workerTasksPending++;

	// Notify worker
	workerTasksAvailable.notify_one();
}

void ResourceManager::dispatchNext()
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

		// Ensure resource loading didn't fail so far
		if (resource->_resourceState != ResourceState::FAILED) {
			// Dispatch resource
			if (resource->dispatchGPU()) {
				resource->_resourceState = ResourceState::READY;
			}
			else {
				resource->_resourceState = ResourceState::FAILED;
			}

			// Release resources data
			resource->releaseData();
		}

		// Pop resource from queue safely
		popSafe(mainTasks);
	}

	// Resource dispatched, prevent another dispatch
	mainDispatchNext = false;

	// Unlock worker mutex and continue worker thread
	workerAwaitingDispatch.notify_one();
}

ResourceManager::WorkerState ResourceManager::readWorkerState() const
{
	return WorkerState(workerActive, workerTarget, workerTasksPending);
}

void ResourceManager::asyncWorker()
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
			resource->_resourceState = ResourceState::LOADING;

			// Sync worker state
			workerTasksPending = workerTasks.size();
			workerActive = true;
			workerTarget = resource;

			// Load resource data
			if (!resource->loadData()) resource->_resourceState = ResourceState::FAILED;

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