#pragma once
namespace Time
{
	// Calculate times for current frame (handled by application context)
	void step(double time);

	// Returns the current application time
	double now();

	// Returns the current application time
	float nowf();

	// Returns the application time of last frame
	double last();

	// Returns the application time of last frame
	float lastf();

	// Returns the current delta time not affected by the time scale
	double unscaledDelta();

	// Returns the current delta time not affected by the time scale
	double unscaledDeltaf();

	// Returns the current delta time
	double delta();

	// Returns the current delta time
	float deltaf();

	// Returns the current time scale
	double getTimeScale();

	// Sets the time scale
	void setTimeScale(double timeScale);
};