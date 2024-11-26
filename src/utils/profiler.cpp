#include "profiler.h"

namespace Profiler {

	std::unordered_map<std::string, std::chrono::steady_clock::time_point> profiles = std::unordered_map<std::string, std::chrono::steady_clock::time_point>();
	std::unordered_map<std::string, double> times = std::unordered_map<std::string, double>();

	bool validateProfile(std::string identifier)
	{
		return profiles.find(identifier) == profiles.end() ? false : true;
	}

	bool validateTime(std::string identifier)
	{
		return times.find(identifier) == times.end() ? false : true;
	}

	std::chrono::steady_clock::time_point now()
	{
		return std::chrono::high_resolution_clock::now();
	}

	void start(std::string identifier)
	{
		profiles[identifier] = now();
	}

	double stop(std::string identifier)
	{
		if (!validateProfile(identifier)) {
			return 0.0;
		}
		double time = std::chrono::duration<double, std::milli>(now() - profiles[identifier]).count();
		times[identifier] = time;
		return time;
	}

	double get(std::string identifier)
	{
		if (!validateTime(identifier)) {
			return 0.0f;
		}
		return times[identifier];
	}

}
