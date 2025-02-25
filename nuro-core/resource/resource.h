#pragma once

#include <string>
#include <atomic>

enum class ResourceState {
	// Resource has not been initialized or loaded yet
	EMPTY,

	// Resource is queued and waiting for asynchronous creation
	QUEUED,

	// Resource is being created (e.g., loading data or being dispatched to the GPU)
	CREATING,

	// Resource has been successfully created and is ready for use
	READY
};

class Resource
{
protected:
	// Protected methods and members are used and managed by resource loader only
	friend class ResourceLoader;

	// Atomic state of the resource
	std::atomic<ResourceState> state;

	// Loads the resources data from I/O
	virtual void loadData() = 0;

	// Releases previously loaded data
	virtual void releaseData() = 0;

	// Creates gpu buffers for resource with previously loaded data
	virtual void dispatchGPU() = 0;

	Resource() : state(ResourceState::EMPTY) {};
	virtual ~Resource() = default;

public:
	// Returns the resources current state
	ResourceState getState() const {
		return state;
	}

	// Returns source path of resource (tmp)
	virtual std::string sourcePath() = 0;
};