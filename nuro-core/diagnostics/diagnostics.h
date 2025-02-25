#pragma once

#include <cstdint>

namespace Diagnostics
{

	void step(); // Prepares diagnostics for next frame

	const float getFps(); // Current fps
	const float getAverageFps(); // Average fps of last fps period

	const uint32_t getCurrentDrawCalls(); // Draw calls issued this frame
	const uint32_t getCurrentVertices(); // Vertices rendered this frame
	const uint32_t getCurrentPolygons(); // Polygons rendered this frame
	const uint32_t getNEntitiesCPU(); // Entities handled on the cpu this frame
	const uint32_t getNEntitiesGPU(); // Entities handled on the gpu this frame

	const void addCurrentDrawCalls(const uint32_t increment);
	const void addCurrentVertices(const uint32_t increment);
	const void addCurrentPolygons(const uint32_t increment);
	const void addNEntitiesCPU(const uint32_t increment);
	const void addNEntitiesGPU(const uint32_t increment);

};