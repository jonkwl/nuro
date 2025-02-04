#pragma once

#include <string>

class ResourceTask
{
public:
	// Loads resources data form the io
	virtual void load() = 0;

	// Creates resources gpu buffers with previously loaded data
	virtual void upload() = 0;
};