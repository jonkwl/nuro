#pragma once

#include "../src/runtime/runtime.h"
#include "../src/camera/camera.h"

class InspectorMode
{
public:
	static float movementSpeed;
	static float sensitivity;

	static void refreshInspector();
};