#pragma once

#include <string>
#include <atomic>
#include <cstdint>

#include <memory/resource_pipe.h>

using ResourceID = uint32_t;

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
private:
	friend class ResourceManager;

	// ID of the resource
	ResourceID _resourceId;

	// Name of resource
	std::string _resourceName;

	// State of the resource
	std::atomic<ResourceState> _resourceState;

protected:
	// Returns a new resource pipe owned by this resource
	ResourcePipe pipe() {
		return std::move(ResourcePipe(_resourceId));
	}

	Resource() : _resourceId(0), _resourceName("none"), _resourceState(ResourceState::EMPTY) {};

public:
	virtual ~Resource() = 0;

	// Returns the resources id
	ResourceID resourceId() const {
		return _resourceId;
	}

	// Returns the resources id
	const std::string& resourceName() const {
		return _resourceName;
	}

	// Returns the resources current state
	ResourceState resourceState() const {
		return _resourceState;
	}
};

inline Resource::~Resource() {}