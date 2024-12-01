#include "time.h"

namespace Time {

	double _time = 0.0;
	double _lastTime = 0.0;
	double _deltaTime = 0.0;

	void step(double time)
	{
		// Update times
		_time = static_cast<float>(time);
		_deltaTime = _time - _lastTime;
		_lastTime = _time;
	}

	const double getTime()
	{
		return _time;
	}

	const double getLastTime()
	{
		return _lastTime;
	}

	const double getDeltaTime()
	{
		return _deltaTime;
	}

	const float fgetTime()
	{
		return static_cast<float>(_time);
	}

	const float fgetLastTime()
	{
		return static_cast<float>(_lastTime);
	}

	const float fgetDeltaTime()
	{
		return static_cast<float>(_deltaTime);
	}

}