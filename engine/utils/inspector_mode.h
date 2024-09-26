#pragma once

#include "../engine/runtime.h"
#include "../engine/objects/camera.h"

class InspectorMode
{
public:
	static float movementSpeed;
	static float sensitivity;

	static void refreshInspector();
};