#include "time.h"

namespace Time {

	double gTime = 0.0;
	double gLastTime = 0.0;
	double gUnscaledDelta = 0.0;
	double gDelta = 0.0;
	double gTimeScale = 1.0;

	void step(double time)
	{
		gTime = static_cast<float>(time);
		gUnscaledDelta = gTime - gLastTime;
		gDelta = gUnscaledDelta * gTimeScale;
		gLastTime = gTime;
	}

	double now()
	{
		return gTime;
	}

	float nowf()
	{
		return static_cast<float>(gTime);
	}

	double last()
	{
		return gLastTime;
	}

	float lastf()
	{
		return static_cast<float>(gLastTime);
	}

	double delta()
	{
		return gDelta;
	}

	float deltaf()
	{
		return static_cast<float>(gDelta);
	}

	double unscaledDelta()
	{
		return gUnscaledDelta;
	}

	double unscaledDeltaf()
	{
		return static_cast<float>(gUnscaledDelta);
	}

	double getTimeScale()
	{
		return gTimeScale;
	}

	void setTimeScale(double timeScale)
	{
		gTimeScale = timeScale;
	}

}