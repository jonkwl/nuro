#include "resource_manager.h"

ResourceManager::ResourceManager() : idCounter(0),
resources(),
asyncPipes(),
asyncPipesSize(0),
processorRunning(false),
processor(),
mtxProcessor(),
processorState(),
cvNextPipe(),
cvAwaitingContext(),
contextNext(false),
contextResult(false),
contextTask(nullptr)
{
}

ResourceManager::~ResourceManager()
{
	// Stop processor
	processorRunning = false;
	if (processor.joinable()) processor.detach();
}

void ResourceManager::updateContext()
{
	// Make sure task is waiting to get executed on context thread
	if (!contextNext) return;

	// Execute context thread task
	if (contextTask)
		contextResult = contextTask();

	// Notify pipe processor
	cvAwaitingContext.notify_all();
}

bool ResourceManager::exec(ResourcePipe&& pipe)
{
	// Start async pipe processing if not running already
	if (!processorRunning) {
		processorRunning = true;
		processor = std::thread(&ResourceManager::asyncPipeProcessor, this);
	}

	// Ensure owner resource is valid
	ResourceRef<Resource> resource = getResource(pipe.owner());
	if (!resource) {
		// Pipe owning resource not available
		Console::out::warning("Resource Manager", "Failed to enqueue execution of pipe, its owner resource with id " + std::to_string(pipe.owner()) + " is invalid");
		return false;
	}

	// Try to enqueue the pipe
	auto pipeHandle = std::make_unique<ResourcePipe>(std::move(pipe));
	bool success = asyncPipes.try_enqueue(std::move(pipeHandle));

	// Update resource status if pipe was enqueued
	if (success) {
		asyncPipesSize++;
		resource->_resourceState = ResourceState::QUEUED;
		cvNextPipe.notify_all();
	}
	else {
		resource->_resourceState = ResourceState::FAILED;
	}

	return success;
}

bool ResourceManager::execAsDependency(ResourcePipe&& pipe)
{
	// Make sure async pipe processing isn't running yet
	if (processorRunning) {
		Console::out::warning("Resource Manager", "Tried to execute a pipe as a dependency, but the async pipe processor is already running");
		return false;
	}

	// Ensure owner resource is valid
	ResourceRef<Resource> resource = getResource(pipe.owner());
	if (!resource) {
		// Pipe owning resource not available
		Console::out::warning("Resource Manager", "Failed to synchronously execute pipe, its owner resource with id " + std::to_string(pipe.owner()) + " is invalid");
		return false;
	}

	// Execute each pipe task synchronously
	while (NextTask nextTask = pipe.next()) {
		ResourceTask task = *nextTask;
		bool success = task.func();
		if (!success) {
			resource->_resourceState = ResourceState::FAILED;
			return false;
		}
	}

	// All pipe tasks were executed successfully
	resource->_resourceState = ResourceState::READY;
	return true;
}

void ResourceManager::asyncPipeProcessor() {

	while (processorRunning) {

		std::unique_lock lock(mtxProcessor);

		// Try to dequeue next pipe
		std::unique_ptr<ResourcePipe> pipe;
		bool nextPipe = asyncPipes.try_dequeue(pipe);

		if (nextPipe) {
			asyncPipesSize--;

			ResourceRef<Resource> resource = getResource(pipe->owner());
			if (!resource) {
				// Pipe owning resource not available
				Console::out::warning("Resource Manager", "Failed to asynchronously execute pipe, its owner resource with id " + std::to_string(pipe->owner()) + " is invalid");
				continue;
			}

			// Update processor state
			processorState.setLoading(resource->resourceName());

			// Execute each pipe task
			resource->_resourceState = ResourceState::LOADING;
			while (NextTask nextTask = pipe->next()) {
				ResourceTask task = *nextTask;

				bool success = false;

				// Execute task on context thread
				if (task.flags & TaskFlags::UseContextThread) {
					contextTask = task.func;		// A
					contextNext = true;				// B
					cvAwaitingContext.wait(lock);	// C
					contextNext = false;			// B
					contextTask = nullptr;			// A
					success = contextResult;
				}
				// Execute task on this thread
				else {
					success = task.func();
				}

				if (!success) {
					resource->_resourceState = ResourceState::FAILED;
					continue;
				}
			}

			// Check if all tasks executed successfully
			if (resource->_resourceState != ResourceState::FAILED)
				resource->_resourceState = ResourceState::READY;
		}
		else {
			processorState.setSleeping();
			cvNextPipe.wait(lock);
		}

	}

}