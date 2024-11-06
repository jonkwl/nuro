#include "camera.h"

Camera::Camera() {
    transform = Transform();

	fov = 70.0f;
	nearClipping = 0.3f;
	farClipping = 1000.0f;
}