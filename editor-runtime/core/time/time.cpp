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

	const double now()
	{
		return _time;
	}

	const double last()
	{
		return _lastTime;
	}

	const double delta()
	{
		return _deltaTime;
	}

	const float nowf()
	{
		return static_cast<float>(_time);
	}

	const float lastf()
	{
		return static_cast<float>(_lastTime);
	}

	const float deltaf()
	{
		return static_cast<float>(_deltaTime);
	}

}