#include <glm.hpp>

#include "../src/rendering/material/imaterial.h"
#include "../src/viewport/viewport.h"
#include "../src/camera/camera.h"

class Model;
class BoundingVolume;
class Entity;

#pragma once
class MeshRenderer
{
public:
	explicit MeshRenderer(Entity* parentEntity);

	Model* model; // Linked model, rendering target

	BoundingVolume* volume; // Bounding volume for the mesh renderer

	std::vector<IMaterial*> materials; // Container for mesh renderers materials

	// Object related post processing settings
	bool useMotionBlur;		   // Enable object motion blur for mesh renderer
	float motionBlurIntensity; // Set objects motion blur intensity

	// Prepare each mesh for upcoming passes for next frame (calculate and cache matrices, perform culling etc.)
	void prepareNextFrame(Camera& renderCamera);

	// Mesh needs to be prepared using prepare() before any pass for current frame is performed
	void forwardPass();
	void prePass();
	void shadowPass();
	void velocityPass();

	static glm::mat4 currentViewMatrix;			  // Static view matrix cache for current frame
	static glm::mat4 currentProjectionMatrix;	  // Static projection matrix cache for current frame
	static glm::mat4 currentViewProjectionMatrix; // Static projection matrix * view matrix cache for current frame
	static glm::mat3 currentViewNormalMatrix;	  // Static transposed inverse view matrix for current frame

	static glm::mat4 currentLightSpaceMatrix; // Static light space matrix cache for current frame
private:
	glm::mat4 currentModelMatrix;  // Model matrix cache for current frame
	glm::mat4 currentMvpMatrix;	   // MVP matrix cache for current frame
	glm::mat4 currentNormalMatrix; // Normal matrix cache for current frame

	glm::mat4 previousModelMatrix; // MVP matrix cache of previous frame (only needed for velocity pass, set after velocity pass)

	Entity* parentEntity; // Linked parent entity

	void render(unsigned int nElements); // Issues draw call

	void performFrustumCulling(Camera& renderCamera);
	bool intersectsFrustum;

	bool isCulled();
};