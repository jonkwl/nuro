#pragma once

#include <string>

class Resource
{
public:
	// Loads assets data from io
	virtual void loadData() = 0;

	// Releases previously loaded data
	virtual void releaseData() = 0;

	// Creates gpu buffers for resource with previously loaded data
	virtual void dispatchGPU() = 0;

	// Returns source path of resource (tmp)
	virtual std::string sourcePath() = 0;
};