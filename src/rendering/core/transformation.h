#pragma once

#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

#include "../src/entity/entity.h"
#include "../src/camera/camera.h"

class Transformation
{
public:
	static glm::vec3 prepareWorldPosition(glm::vec3 position); // Inverses some of the input position componenets for unity like positioning
	static glm::vec3 prepareWorldRotation(glm::vec3 rotation); // Inverses some of the input rotation componenets for unity like rotation

	static glm::mat4 modelMatrix(Entity* entity); // Model matrix
	static glm::mat4 viewMatrix(Camera* camera); // View matrix
	static glm::mat4 projectionMatrix(Camera* camera, int width, int height); // Projection matrix
	  
	static glm::mat4 lightViewMatrix(glm::vec3 lightPosition, glm::vec3 lightDirection);
	static glm::mat4 lightProjectionMatrix(Camera* camera, float boundsWidth, float boundsHeight);
};