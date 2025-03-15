#pragma once

#include <cstdint>

class Mesh;

namespace Shapes {

	// Creates a static plane mesh synchronously
	Mesh* createPlane();

	// Creates a static cube mesh synchronously
	Mesh* createCube();

}