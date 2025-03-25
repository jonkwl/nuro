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

	// All resources are deconstructed automatically
}

void ResourceManager::loadSync(uint32_t resourceId)
{
	// Retrieve resource
	OptResource<Resource> optResource = getResource(resourceId);
	if (!optResource) {
		Console::out::warning("Resource Manager", "Tried to load invalid resource synchronously (ID: " + std::to_string(resourceId) + ")");
		return;
	}
	ResourceRef<Resource> resource = *optResource;

	// Load resources data
	resource->_loadedIoData = resource->loadIoData();
	if (!resource->_loadedIoData) {
		resource->_resourceState = ResourceState::FAILED;
		return;
	}

	// Upload resource buffers
	if (!resource->uploadBuffers()) {
		resource->_resourceState = ResourceState::FAILED;
		resource->freeIoData();
		return;
	}

	tryFreeIoData(resource);

	resource->_resourceState = ResourceState::READY;
}

void ResourceManager::loadAsync(uint32_t resourceId)
{
	// Retrieve resource
	OptResource<Resource> optResource = getResource(resourceId);
	if (!optResource) {
		Console::out::warning("Resource Manager", "Failed to load invalid resource asynchronously (ID: " + std::to_string(resourceId) + ")");
		return;
	}
	ResourceRef<Resource> resource = *optResource;

	// Add task to load tasks queue
	if (!workerTasks.try_enqueue(resourceId)) {
		Console::out::warning("Resource Manager", "Failed to enqueue resource on main thread (ID: " + std::to_string(resourceId) + ")");
		resource->_resourceState = ResourceState::FAILED;
		return;
	}
	workerTasksSize++;

	// Update resources state
	resource->_resourceState = ResourceState::QUEUED;

	// Notify worker
	workerTasksAvailable.notify_one();
}

void ResourceManager::deleteBuffers(uint32_t resourceId)
{
	// Retrieve resource
	OptResource<Resource> optResource = getResource(resourceId);
	if (!optResource) {
		Console::out::warning("Resource Manager", "Failed to delete buffers of invalid resource (ID: " + std::to_string(resourceId) + ")");
		return;
	}
	ResourceRef<Resource> resource = *optResource;

	// Delete resources buffers if available
	if (resource->_resourceState == ResourceState::READY) {
		resource->deleteBuffers();
		resource->_resourceState = ResourceState::EMPTY;
	}
}

void ResourceManager::dispatchNext()
{
	// [MAIN THREAD]

	// Only dispatch next resource if allowed by worker before
	if (!mainDispatchNext) return;

	// Make sure worker thread is locked during dispatch (since main tasks is a shared resource)
	std::unique_lock<std::mutex> lock(workerMtx);

	if (!lock.owns_lock()) Console::out::warning("Resource Manager", "Couldn't lock worker mutex on main thread");
	
	// Dispatch next resource in queue (only one per frame to prevent heavy main thread blocking)
	if (mainTasksSize > 0) {
		// Get queued resource id
		uint32_t resourceId;
		if (!mainTasks.try_dequeue(resourceId)) {
			Console::out::warning("Resource Manager", "Failed to dequeue resource on main thread (ID: " + std::to_string(resourceId) + ")");
			return;
		}
		mainTasksSize--;

		// Try to fetch resource
		OptResource<Resource> optResource = getResource(resourceId);
		if (!optResource) {
			Console::out::warning("Resource Manager", "Failed to dispatch invalid resource (ID: " + std::to_string(resourceId) + ")");
			return;
		}
		ResourceRef<Resource> resource = *optResource;

		// Ensure resource loading didn't fail so far
		if (resource->_resourceState != ResourceState::FAILED) {
			// Dispatch resource
			if (resource->uploadBuffers()) {
				resource->_resourceState = ResourceState::READY;
			}
			else {
				resource->_resourceState = ResourceState::FAILED;
			}

			tryFreeIoData(resource);
		}
	}

	// Resource dispatched, prevent another dispatch
	mainDispatchNext = false;

	// Unlock worker mutex and continue worker thread
	workerAwaitingDispatch.notify_one();
}

ResourceManager::WorkerState ResourceManager::readWorkerState() const
{
	return WorkerState(workerActive, workerTarget, workerTasksSize);
}

void ResourceManager::asyncWorker()
{
	// [WORKER THREAD]

	while (running) {

		std::unique_lock<std::mutex> lock(workerMtx);

		// Condition: Queue with tasks to load must not be empty
		if (workerTasksSize == 0) {
			workerTasksAvailable.wait(lock, [&]() { return workerTasksSize > 0; });
		}

		while (workerTasksSize > 0) {
			// Get queued resource id
			uint32_t resourceId;
			if (!workerTasks.try_dequeue(resourceId)) {
				Console::out::warning("Resource Manager", "Failed to dequeue resource on worker thread (ID: " + std::to_string(resourceId) + ")");
				continue;
			}
			workerTasksSize--;

			// Try to fetch resource
			OptResource<Resource> optResource = getResource(resourceId);
			if (!optResource) {
				Console::out::warning("Resource Manager", "Failed to upload buffers of invalid resource (ID: " + std::to_string(resourceId) + ")");
				continue;
			}
			ResourceRef<Resource> resource = *optResource;

			// Update resources state
			resource->_resourceState = ResourceState::LOADING;

			// Sync worker state
			workerActive = true;
			workerTarget = resourceId;

			// Load resource data
			if (!resource->loadIoData()) {
				resource->_resourceState = ResourceState::FAILED;
			}
			else {
				resource->_loadedIoData = true;
			}

			// Update queues
			if (!mainTasks.try_enqueue(resourceId)) {
				Console::out::warning("Resource Manager", "Failed to enqueue resource on worker thread (ID: " + std::to_string(resourceId) + ")");
				continue;
			}
			mainTasksSize++;

			// Condition: Main thread must have dispatched pending resource before worker continues
			mainDispatchNext = true;
			workerAwaitingDispatch.wait(lock, [&]() { return !mainDispatchNext; });
		}

		// Sync worker state
		workerActive = false;
		workerTarget = 0;
	}
}

void ResourceManager::tryFreeIoData(ResourceRef<Resource>& resource)
{
	if (!resource->preserveIoData) {
		resource->freeIoData();
		resource->_loadedIoData = false;
	}
}