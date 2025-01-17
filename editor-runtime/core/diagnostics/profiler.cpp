#include "profiler.h"

namespace Profiler
{

	std::unordered_map<std::string, std::chrono::steady_clock::time_point> gProfiles = std::unordered_map<std::string, std::chrono::steady_clock::time_point>();
	std::unordered_map<std::string, double> gTimes = std::unordered_map<std::string, double>();

	bool _validateProfile(const std::string& identifier)
	{
		return gProfiles.find(identifier) == gProfiles.end() ? false : true;
	}

	bool _validateTime(const std::string& identifier)
	{
		return gTimes.find(identifier) == gTimes.end() ? false : true;
	}

	std::chrono::steady_clock::time_point _now()
	{
		return std::chrono::high_resolution_clock::now();
	}

	void start(const std::string& identifier)
	{
		gProfiles[identifier] = _now();
	}

	double stop(const std::string& identifier)
	{
		if (!_validateProfile(identifier))
		{
			return 0.0;
		}
		double time = std::chrono::duration<double, std::micro>(_now() - gProfiles[identifier]).count();
		gTimes[identifier] = time;
		return time;
	}

	double getMs(const std::string& identifier)
	{
		if (!_validateTime(identifier)) return 0.0f;
		return gTimes[identifier] * 0.001;
	}

	double getUs(const std::string& identifier)
	{
		if (!_validateTime(identifier)) return 0.0f;
		return gTimes[identifier];
	}

	double getNs(const std::string& identifier)
	{
		if (!_validateTime(identifier)) return 0.0f;
		return gTimes[identifier] * 1000;
	}

}
