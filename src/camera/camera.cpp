#include "camera.h"

#include "../src/window/window.h"

Camera::Camera() {
    transform = Transform();

    frustum = Frustum();

	fov = 70.0f;
	near = 0.3f;
	far = 1000.0f;
}