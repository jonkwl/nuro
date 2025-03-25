#include "resource_manager.h"

ResourceManager::ResourceManager() : idCounter(0),
resources()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::update()
{
}

bool ResourceManager::execSync(ResourcePipe pipe)
{
	OptResource<Resource> owner = getResource(pipe.owner());
	if (!owner) {
		// Pipe owning resource not available
		Console::out::warning("Resource Manager", "Failed to synchronously execute pipe, its owner resource with id " + std::to_string(pipe.owner()) + " is invalid!");
		return false;
	}

	// Fetch resource owning the pipe
	ResourceRef<Resource> resource = *owner;

	// Execute each pipe task synchronously
	while (NextTask next = pipe.next()) {
		ResourceTask task = *next;
		bool success = task.func();

		// Cancel further execution of pipe if last task failed
		if (!success) {
			resource->_resourceState = ResourceState::FAILED;
			return false;
		}
	}

	// All pipe tasks were executed successfully
	resource->_resourceState = ResourceState::READY;
	return true;
}

bool ResourceManager::execAsync(ResourcePipe pipe)
{
	OptResource<Resource> owner = getResource(pipe.owner());
	if (!owner) {
		// Pipe owning resource not available
		Console::out::warning("Resource Manager", "Failed to enqueue execution of pipe, its owner resource with id " + std::to_string(pipe.owner()) + " is invalid!");
		return false;
	}

	// Fetch resource owning the pipe
	ResourceRef<Resource> resource = *owner;

	Console::out::info("Resource Manager", "Enqueued execution of pipe owned by resource \"" + resource->resourceName() + "\"");
	return true;
}