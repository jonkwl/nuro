#pragma once
namespace Diagnostics
{

	void step(); // Prepares diagnostics for next frame

	const float getFps(); // Current fps
	const float getAverageFps(); // Average fps of last fps period

	const unsigned int getCurrentDrawCalls(); // Draw calls issued this frame
	const unsigned int getCurrentVertices(); // Vertices rendered this frame
	const unsigned int getCurrentPolygons(); // Polygons rendered this frame
	const unsigned int getNEntitiesCPU(); // Entities handled on the cpu this frame
	const unsigned int getNEntitiesGPU(); // Entities handled on the gpu this frame

	const void addCurrentDrawCalls(const unsigned int increment);
	const void addCurrentVertices(const unsigned int increment);
	const void addCurrentPolygons(const unsigned int increment);
	const void addNEntitiesCPU(const unsigned int increment);
	const void addNEntitiesGPU(const unsigned int increment);

};