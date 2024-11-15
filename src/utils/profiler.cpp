#include "profiler.h"

std::unordered_map<std::string, std::chrono::steady_clock::time_point> Profiler::profiles = std::unordered_map<std::string, std::chrono::steady_clock::time_point>();
std::unordered_map<std::string, double> Profiler::times = std::unordered_map<std::string, double>();

std::chrono::steady_clock::time_point Profiler::now()
{
	return std::chrono::high_resolution_clock::now();
}

void Profiler::start(std::string identifier)
{
	profiles[identifier] = now();
}

double Profiler::stop(std::string identifier)
{
	if (!validateProfile(identifier)) {
		return 0.0;
	}
	double time = std::chrono::duration<double, std::milli>(now() - profiles[identifier]).count();
	times[identifier] = time;
	return time;
}

double Profiler::get(std::string identifier)
{
	if (!validateTime(identifier)) {
		return 0.0f;
	}
	return times[identifier];
}

bool Profiler::validateProfile(std::string identifier)
{
	return profiles.find(identifier) == profiles.end() ? false : true;
}

bool Profiler::validateTime(std::string identifier)
{
	return times.find(identifier) == times.end() ? false : true;
}
