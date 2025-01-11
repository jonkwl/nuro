#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

namespace Profiler
{

	// Creates new profile for given identifier and starts profiling
	void start(const std::string& identifier);

	// Stops profiling for given identifier and returns time
	double stop(const std::string& identifier);

	// Returns last cached time for given identifier in milliseconds
	double getMs(const std::string& identifier);

	// Returns last cached time for given identifier in microseconds
	double getUs(const std::string& identifier);

	// Returns last cached time for given identifier in nanoseconds
	double getNs(const std::string& identifier);

};