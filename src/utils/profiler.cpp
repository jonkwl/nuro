#include "profiler.h"

namespace Profiler
{

	std::unordered_map<std::string, std::chrono::steady_clock::time_point> _profiles = std::unordered_map<std::string, std::chrono::steady_clock::time_point>();
	std::unordered_map<std::string, double> _times = std::unordered_map<std::string, double>();

	bool _validateProfile(std::string identifier)
	{
		return _profiles.find(identifier) == _profiles.end() ? false : true;
	}

	bool _validateTime(std::string identifier)
	{
		return _times.find(identifier) == _times.end() ? false : true;
	}

	std::chrono::steady_clock::time_point _now()
	{
		return std::chrono::high_resolution_clock::now();
	}

	void start(std::string identifier)
	{
		_profiles[identifier] = _now();
	}

	double stop(std::string identifier)
	{
		if (!_validateProfile(identifier))
		{
			return 0.0;
		}
		double time = std::chrono::duration<double, std::milli>(_now() - _profiles[identifier]).count();
		_times[identifier] = time;
		return time;
	}

	double get(std::string identifier)
	{
		if (!_validateTime(identifier))
		{
			return 0.0f;
		}
		return _times[identifier];
	}

}
