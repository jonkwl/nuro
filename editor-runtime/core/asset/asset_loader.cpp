#include "asset_loader.h"

#include <chrono>

#include "../core/utils/console.h"

AssetLoader::AssetLoader() : workerState()
{
	// Launch worker
	running.store(true);
	worker = std::jthread(&AssetLoader::asyncWorker, this);
}

AssetLoader::~AssetLoader()
{
	// Stop worker
	running.store(false);
	worker.request_stop();
}

const AssetLoader::WorkerState& AssetLoader::readWorkerState() const
{
	return workerState;
}

void AssetLoader::createSync(Asset* asset)
{
	asset->load();
	asset->upload();
}

void AssetLoader::createAsync(Asset* asset)
{
	addAsyncTask(asset);
}

void AssetLoader::update()
{
	while (!uploadTasks.empty()) {
		Console::out::processInfo("UPLOADING SOME RESOURCE SYNCHRONOUSLY");

		Asset* task = uploadTasks.front();
		task->upload();
		uploadTasks.pop();
	}
}

void AssetLoader::addAsyncTask(Asset* task)
{
	// Add task to load tasks queue
	loadTasks.push(task);

	// Notify worker
	workerCv.notify_all();
}

void AssetLoader::asyncWorker()
{
	while (running.load()) {

		std::unique_lock<std::mutex> lock(workerMtx);

		// Condition: Queue with tasks to load must not be empty
		if (loadTasks.empty()) {
			workerCv.wait(lock);
		}

		while (!loadTasks.empty()) {
			// Tmp
			std::this_thread::sleep_for(std::chrono::seconds(1));

			// Fetch asset to be loaded
			Asset* task = loadTasks.front();

			// Sync current asset in worker state
			workerState.currentTask = task;

			// Load asset
			task->load();

			// Update queues
			loadTasks.pop();
			uploadTasks.push(task);

			// Update pending task amount
			workerState.tasksPending = loadTasks.size();
		}

		// Clear current task from worker
		workerState.currentTask = nullptr;
	}
}