#pragma once

#include <string>

class Asset
{
public:
	// Loads assets data form the io
	virtual void load() = 0;

	// Creates assets gpu buffers with previously loaded data
	virtual void upload() = 0;

	// Returns assets source path (tmp)
	virtual std::string sourcePath() = 0;
};