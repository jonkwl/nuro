#include <glm.hpp>

#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/shader/Shader.h"
#include "../core/viewport/viewport.h"
#include "../core/camera/camera.h"

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
	void prePass(Shader* shader);
	void shadowPass(Shader* shader);
	void velocityPass(Shader* shader);

	static glm::mat4 currentViewMatrix;			  // Static view matrix cache for current frame
	static glm::mat4 currentProjectionMatrix;	  // Static projection matrix cache for current frame
	static glm::mat4 currentViewProjectionMatrix; // Static projection matrix * view matrix cache for current frame
	static glm::mat3 currentViewNormalMatrix;	  // Static transposed inverse view matrix for current frame

	static glm::mat4 currentLightSpaceMatrix; // Static light space matrix cache for current frame

	void recalculateRenderMatrices(); // Recalculates the current model matrix according to transform
private:
	glm::mat4 currentModelMatrix;  // Model matrix cache for current frame
	glm::mat4 currentMvpMatrix;	   // MVP matrix cache for current frame
	glm::mat4 currentNormalMatrix; // Normal matrix cache for current frame
	glm::mat4 previousModelMatrix; // MVP matrix cache of previous frame (only needed for velocity pass, set after velocity pass)

	Entity* parentEntity; // Linked parent entity

	// Can be optimized, not every mesh renderer needs to hold this pointer
	Shader* materialUnavailableShader; // Shader used for unavailable material

	void render(unsigned int nElements); // Issues draw call

	void performFrustumCulling(Camera& renderCamera);
	bool intersectsFrustum;

	bool isCulled();
};