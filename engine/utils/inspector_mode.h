#pragma once

#include "../engine/runtime/runtime.h"
#include "../engine/camera/camera.h"

class InspectorMode
{
public:
	static float movementSpeed;
	static float sensitivity;

	static void refreshInspector();
};