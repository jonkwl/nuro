#include "diagnostics.h"

#include <time/time.h>

namespace Diagnostics {

	float gFps = 0.0f;
	float gAverageFps = 0.0f;
	float gAverageFpsUpdateTime = 1.0f; // Duration of each fps period
	float gAverageFpsFrameCount = 0.0f; // Frame counter for average fps calculation
	float gAverageFpsElapsedTime = 0.0f; // Elapsed time since beginning of last fps period

	uint32_t gCurrentDrawCalls = 0;
	uint32_t gCurrentVertices = 0;
	uint32_t gCurrentPolygons = 0;

	uint32_t gNCPUEntities = 0;
	uint32_t gNGPUEntities = 0;

	void step()
	{
		// Get values needed for step
		float deltaTime = Time::deltaf();

		// Reset frame based diagnostics
		gCurrentDrawCalls = 0;
		gCurrentVertices = 0;
		gCurrentPolygons = 0;

		gNCPUEntities = 0;
		gNGPUEntities = 0;

		// Calculate current fps
		gFps = 1.0 / deltaTime;

		// Calculate current average fps
		gAverageFpsElapsedTime += deltaTime;
		gAverageFpsFrameCount += 1.0f;
		if (gAverageFpsElapsedTime >= gAverageFpsUpdateTime)
		{
			gAverageFps = gAverageFpsFrameCount / gAverageFpsElapsedTime;
			gAverageFpsElapsedTime = 0.0f;
			gAverageFpsFrameCount = 0;
		}
	}

	const float getFps()
	{
		return gFps;
	}

	const float getAverageFps()
	{
		return gAverageFps;
	}

	const uint32_t getCurrentDrawCalls()
	{
		return gCurrentDrawCalls;
	}

	const uint32_t getCurrentVertices()
	{
		return gCurrentVertices;
	}

	const uint32_t getCurrentPolygons()
	{
		return gCurrentPolygons;
	}

	const uint32_t getNEntitiesCPU()
	{
		return gNCPUEntities;
	}

	const uint32_t getNEntitiesGPU()
	{
		return gNGPUEntities;
	}

	const void addCurrentDrawCalls(const uint32_t increment)
	{
		gCurrentDrawCalls += increment;
	}

	const void addCurrentVertices(const uint32_t increment)
	{
		gCurrentVertices += increment;
	}

	const void addCurrentPolygons(const uint32_t increment)
	{
		gCurrentPolygons += increment;
	}

	const void addNEntitiesCPU(const uint32_t increment)
	{
		gNCPUEntities += increment;
	}

	const void addNEntitiesGPU(const uint32_t increment)
	{
		gNGPUEntities += increment;
	}

}