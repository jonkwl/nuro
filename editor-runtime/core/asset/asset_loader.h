#pragma once

#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../core/asset/asset.h"

class AssetLoader
{
public:
	struct WorkerState {
		Asset* currentTask;
		uint32_t tasksPending = 0;
	};

	AssetLoader();
	~AssetLoader();

	// Returns the read-only state of the worker
	const WorkerState& readWorkerState() const;

	// Creates asset synchronously, blocking until complete  
	void createSync(Asset* asset);

	// Queues asset creation for asynchronous creation, not blocking
	void createAsync(Asset* asset);

	// Perform pending actions on main thread [performs asset uploads viable for this frame synchronously]
	void update();

private:
	// Adds a resource task to the async queue
	void addAsyncTask(Asset* task);

	// Async worker thread
	void asyncWorker();

private:
	std::atomic<bool> running;

	// Queues
	std::queue<Asset*> loadTasks;
	std::queue<Asset*> uploadTasks;
	
	// Worker thread
	std::jthread worker;
	std::mutex workerMtx;
	std::condition_variable workerCv;

	// State of the worker, only to be changed by worker thread
	WorkerState workerState;
};