#include "time.h"

namespace Time {

	double gTime = 0.0;
	double gLastTime = 0.0;
	double gDeltaTime = 0.0;

	void step(double time)
	{
		// Update times
		gTime = static_cast<float>(time);
		gDeltaTime = gTime - gLastTime;
		gLastTime = gTime;
	}

	const double now()
	{
		return gTime;
	}

	const double last()
	{
		return gLastTime;
	}

	const double delta()
	{
		return gDeltaTime;
	}

	const float nowf()
	{
		return static_cast<float>(gTime);
	}

	const float lastf()
	{
		return static_cast<float>(gLastTime);
	}

	const float deltaf()
	{
		return static_cast<float>(gDeltaTime);
	}

}