#pragma once
namespace Time
{
	void step(double time); // Calculate times for current frame, provide context application start time

	const double getTime(); // Get current application time
	const double getLastTime(); // Get application time of last frame
	const double getDeltaTime(); // Get current delta time

	const float fgetTime(); // As float
	const float fgetLastTime(); // As float
	const float fgetDeltaTime(); // As float

};