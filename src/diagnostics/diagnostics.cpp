#include "diagnostics.h"

#include "../src/time/time.h"

namespace Diagnostics {

	float _fps = 0.0f;
	float _averageFps = 0.0f;
	float _averageFpsUpdateTime = 1.0f; // Duration of each fps period
	float _averageFpsFrameCount = 0.0f; // Frame counter for average fps calculation
	float _averageFpsElapsedTime = 0.0f; // Elapsed time since beginning of last fps period

	unsigned int _currentDrawCalls = 0;
	unsigned int _currentVertices = 0;
	unsigned int _currentPolygons = 0;

	unsigned int _nCPUEntities = 0;
	unsigned int _nGPUEntities = 0;

	void step()
	{
		// Get values needed for step
		float deltaTime = Time::fgetDeltaTime();

		// Reset frame based diagnostics
		_currentDrawCalls = 0;
		_currentVertices = 0;
		_currentPolygons = 0;

		_nCPUEntities = 0;
		_nGPUEntities = 0;

		// Calculate current fps
		_fps = 1.0 / deltaTime;

		// Calculate current average fps
		_averageFpsElapsedTime += deltaTime;
		_averageFpsFrameCount += 1.0f;
		if (_averageFpsElapsedTime >= _averageFpsUpdateTime)
		{
			_averageFps = _averageFpsFrameCount / _averageFpsElapsedTime;
			_averageFpsElapsedTime = 0.0f;
			_averageFpsFrameCount = 0;
		}
	}

	const float getFps()
	{
		return _fps;
	}

	const float getAverageFps()
	{
		return _averageFps;
	}

	const unsigned int getCurrentDrawCalls()
	{
		return _currentDrawCalls;
	}

	const unsigned int getCurrentVertices()
	{
		return _currentVertices;
	}

	const unsigned int getCurrentPolygons()
	{
		return _currentPolygons;
	}

	const unsigned int getNEntitiesCPU()
	{
		return _nCPUEntities;
	}

	const unsigned int getNEntitiesGPU()
	{
		return _nGPUEntities;
	}

	const void addCurrentDrawCalls(const unsigned int increment)
	{
		_currentDrawCalls += increment;
	}

	const void addCurrentVertices(const unsigned int increment)
	{
		_currentVertices += increment;
	}

	const void addCurrentPolygons(const unsigned int increment)
	{
		_currentPolygons += increment;
	}

	const void addNEntitiesCPU(const unsigned int increment)
	{
		_nCPUEntities += increment;
	}

	const void addNEntitiesGPU(const unsigned int increment)
	{
		_nGPUEntities += increment;
	}

}