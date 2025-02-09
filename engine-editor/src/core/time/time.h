#pragma once
namespace Time
{
	// Calculate times for current frame (handled by application context)
	void step(double time);

	// Get current application time as double
	const double now();

	// Get application time of last frame as double
	const double last();

	// Get current delta time as double
	const double delta();

	// Get current application time as float
	const float nowf();

	// Get application time of last frame as float
	const float lastf();

	// Get current delta time as float
	const float deltaf();

};