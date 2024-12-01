#pragma once

#include <vector>
#include <glm.hpp>

#include "../src/viewport/viewport.h"

class Entity;
class Camera;

namespace Transformation
{
	glm::vec3 prepareWorldPosition(glm::vec3 position); // Inverses some of the input position componenets for unity like positioning
	glm::vec3 prepareWorldRotation(glm::vec3 rotation); // Inverses some of the input rotation componenets for unity like rotation
	
	glm::mat4 modelMatrix(Entity* entity);							  // Model matrix
	glm::mat4 viewMatrix(Camera& camera);							  // View matrix
	glm::mat4 projectionMatrix(Camera& camera, Viewport& viewport); // Projection matrix
	
	glm::mat4 lightViewMatrix(glm::vec3 lightPosition, glm::vec3 lightDirection);
	glm::mat4 lightProjectionMatrix(float boundsWidth, float boundsHeight, float near, float far);
};