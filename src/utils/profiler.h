#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

namespace Profiler
{
	void start(std::string identifier);	 // Creates new profile for given identifier and starts profiling
	double stop(std::string identifier); // Stops profiling for given identifier and returns time

	double get(std::string identifier); // Returns last cached time for given identifier
};