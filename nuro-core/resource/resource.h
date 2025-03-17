#pragma once

#include <string>
#include <atomic>
#include <cstdint>

enum class ResourceState {
	// Resource has not been initialized or loaded yet
	EMPTY,

	// Resource is queued and waiting for asynchronous creation
	QUEUED,

	// Resource is being loaded (e.g., loading data or being dispatched to the GPU)
	LOADING,

	// Resource has been successfully loaded and is ready for use
	READY,

	// Laoding resource failed
	FAILED
};

class Resource
{
protected:
	// Protected methods and members are used and managed by resource loader only
	friend class ResourceManager;

	// ID of the resource
	uint32_t _resourceId;

	// Atomic state of the resource
	std::atomic<ResourceState> _resourceState;

	// Loads the resources data from I/O
	virtual bool loadData() = 0;

	// Releases previously loaded data
	virtual bool releaseData() = 0;

	// Creates gpu buffers for resource with previously loaded data
	virtual bool dispatchGPU() = 0;

	Resource() : _resourceId(0), _resourceState(ResourceState::EMPTY) {};
	virtual ~Resource() = default;

public: 
	// Returns the resources id
	uint32_t resourceId() const {
		return _resourceId;
	}

	// Returns the resources current state
	ResourceState resourceState() const {
		return _resourceState;
	}

	// Returns source path of resource (tmp)
	virtual std::string sourcePath() = 0;
};