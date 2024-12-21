#include "diagnostics.h"

#include "../core/time/time.h"

namespace Diagnostics {

	float _fps = 0.0f;
	float _averageFps = 0.0f;
	float _averageFpsUpdateTime = 1.0f; // Duration of each fps period
	float _averageFpsFrameCount = 0.0f; // Frame counter for average fps calculation
	float _averageFpsElapsedTime = 0.0f; // Elapsed time since beginning of last fps period

	uint32_t _currentDrawCalls = 0;
	uint32_t _currentVertices = 0;
	uint32_t _currentPolygons = 0;

	uint32_t _nCPUEntities = 0;
	uint32_t _nGPUEntities = 0;

	void step()
	{
		// Get values needed for step
		float deltaTime = Time::deltaf();

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

	const uint32_t getCurrentDrawCalls()
	{
		return _currentDrawCalls;
	}

	const uint32_t getCurrentVertices()
	{
		return _currentVertices;
	}

	const uint32_t getCurrentPolygons()
	{
		return _currentPolygons;
	}

	const uint32_t getNEntitiesCPU()
	{
		return _nCPUEntities;
	}

	const uint32_t getNEntitiesGPU()
	{
		return _nGPUEntities;
	}

	const void addCurrentDrawCalls(const uint32_t increment)
	{
		_currentDrawCalls += increment;
	}

	const void addCurrentVertices(const uint32_t increment)
	{
		_currentVertices += increment;
	}

	const void addCurrentPolygons(const uint32_t increment)
	{
		_currentPolygons += increment;
	}

	const void addNEntitiesCPU(const uint32_t increment)
	{
		_nCPUEntities += increment;
	}

	const void addNEntitiesGPU(const uint32_t increment)
	{
		_nGPUEntities += increment;
	}

}