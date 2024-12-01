#pragma once

#include <vector>
#include <glm.hpp>

#include "../src/viewport/viewport.h"
#include "../src/camera/camera.h"

class Entity;

namespace Transformation
{
	glm::vec3 prepareWorldPosition(const glm::vec3 position); // Inverses some of the input position componenets for unity like positioning
	glm::vec3 prepareWorldRotation(const glm::vec3 rotation); // Inverses some of the input rotation componenets for unity like rotation
	
	glm::mat4 modelMatrix(Entity* entity);							  // Model matrix
	glm::mat4 viewMatrix(Camera& camera);							  // View matrix
	glm::mat4 projectionMatrix(Camera& camera, const Viewport& viewport); // Projection matrix
	
	glm::mat4 lightViewMatrix(const glm::vec3 lightPosition, const glm::vec3 lightDirection);
	glm::mat4 lightProjectionMatrix(const float boundsWidth, const float boundsHeight, const float near, const float far);
};