#pragma once
namespace Time
{
	void step(double time); // Calculate times for current frame, provide context application start time

	const double now(); // Get current application time
	const double last(); // Get application time of last frame
	const double delta(); // Get current delta time

	const float nowf(); // As float
	const float lastf(); // As float
	const float deltaf(); // As float

};