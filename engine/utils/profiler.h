#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

class Profiler
{
public:
	static std::chrono::steady_clock::time_point now();

	static void start(std::string identifier); // Creates new profile for given identifier and starts profiling
	static double stop(std::string identifier); // Stops profiling for given identifier and returns time

	static double get(std::string identifier); // Returns last cached time for given identifier
private:
	static bool validateProfile(std::string identifier); // Validates if profile by given identifier exists
	static bool validateTime(std::string identifier); // Validates if cached time for given identifier exists

	static std::unordered_map<std::string, std::chrono::steady_clock::time_point> profiles;
	static std::unordered_map<std::string, double> times;
};