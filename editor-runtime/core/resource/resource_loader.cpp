#include "resource_loader.h"

#include <chrono>

#include "../core/utils/console.h"

ResourceLoader::ResourceLoader() : workerState()
{
	// Launch worker
	running.store(true);
	worker = std::jthread(&ResourceLoader::asyncWorker, this);
}

ResourceLoader::~ResourceLoader()
{
	// Stop worker
	running.store(false);
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
	addAsyncTask(resource);
}

void ResourceLoader::update()
{
	// [MAIN THREAD]
	
	// Dispatch next resource in queue (max one resource upload per update to prevent extreme frame stalling)
	if (!mainTasks.empty()) {
		Resource* task = mainTasks.front();
		task->dispatchGPU();
		task->releaseData();
		mainTasks.pop();
	}
}

void ResourceLoader::addAsyncTask(Resource* task)
{
	// Add task to load tasks queue
	workerTasks.push(task);

	// Notify worker
	workerCv.notify_all();
}

void ResourceLoader::asyncWorker()
{
	// [WORKER THREAD]

	// TODO: ADD MORE LOCKS TO PREVENT DATA RACES!

	while (running.load()) {

		std::unique_lock<std::mutex> lock(workerMtx);

		// Condition: Queue with tasks to load must not be empty
		if (workerTasks.empty()) {
			workerCv.wait(lock);
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
			workerTasks.pop();
			mainTasks.push(resource);
		}

		// Clear worker state
		workerState.workerActive = false;
		workerState.currentResource = nullptr;
		workerState.resourcesPending = 0;
	}
}