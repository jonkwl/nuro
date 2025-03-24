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
private:
	friend class ResourceManager;

	// ID of the resource
	uint32_t _resourceId;

	// Name of resource
	std::string _resourceName;

	// Atomic state of the resource (indicating its usability rather than the status of its data)
	std::atomic<ResourceState> _resourceState;

	// Atomic flag set if resources io data is loaded
	std::atomic<bool> _loadedIoData;

protected:
	// Flag setting if io data should be preserved after uploading resource
	bool preserveIoData;

	// Loads the resources data from io
	virtual bool loadIoData() = 0;

	// Frees any previously loaded io data
	virtual void freeIoData() = 0;

	// Creates resources target buffers (e.g. on the gpu) with previously loaded data
	virtual bool uploadBuffers() = 0;

	// Deletes resources previously uploaded target buffers
	virtual void deleteBuffers() = 0;

public: 
	Resource() : _resourceId(0), _resourceName("none"), _resourceState(ResourceState::EMPTY), _loadedIoData(false), preserveIoData(false) {};
	virtual ~Resource() = default;

	// Returns the resources id
	uint32_t resourceId() const {
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

	// Returns if the resources io data is loaded
	bool loadedIoData() const {
		return _loadedIoData;
	}
};