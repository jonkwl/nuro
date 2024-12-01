#include "mesh_renderer.h"

#include <glad/glad.h>

#include "../src/rendering/model/model.h"
#include "../src/rendering/model/mesh.h"
#include "../src/rendering/culling/bounding_volume.h"
#include "../src/entity/entity.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/diagnostics/diagnostics.h"
#include "../src/runtime/runtime.h"

glm::mat4 MeshRenderer::currentViewMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentProjectionMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentViewProjectionMatrix = glm::mat4(1.0);
glm::mat3 MeshRenderer::currentViewNormalMatrix = glm::mat3(1.0);

glm::mat4 MeshRenderer::currentLightSpaceMatrix = glm::mat4(1.0);

MeshRenderer::MeshRenderer(Entity* parentEntity) : parentEntity(parentEntity),
model(nullptr),
volume(new BoundingAABB()),
useMotionBlur(false),
motionBlurIntensity(1.0f),
currentModelMatrix(glm::mat4(1.0f)),
currentMvpMatrix(glm::mat4(1.0f)),
currentNormalMatrix(glm::mat4(1.0f)),
previousModelMatrix(glm::mat4(1.0f)),
intersectsFrustum(false)
{
}

void MeshRenderer::prepareNextFrame()
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	// Calculate and cache model and mvp matrix for current frame
	currentModelMatrix = Transformation::modelMatrix(parentEntity);
	currentMvpMatrix = currentViewProjectionMatrix * currentModelMatrix;
	currentNormalMatrix = glm::transpose(glm::inverse(currentModelMatrix));

	// Frustum culling
	performFrustumCulling();
}

void MeshRenderer::forwardPass(Viewport& viewport)
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	// Check if render target was culled this frame -> cancel
	if (isCulled())
		return;

	// Render each mesh of entity
	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Get mesh material by index
		IMaterial* material = nullptr;

		unsigned int materialIndex = mesh.getMaterialIndex();
		// Try find material by models material index
		if (materialIndex < materials.size())
		{
			// Available material found -> use material
			material = materials[materialIndex];
		}

		// No available material found -> use default material
		if (material == nullptr)
		{
			material = Runtime::defaultMaterial;
		}

		// Bind material
		material->bind(viewport);

		// Set shader uniforms
		Shader* shader = material->getShader();
		shader->setMatrix4("mvpMatrix", currentMvpMatrix);
		shader->setMatrix4("modelMatrix", currentModelMatrix);
		shader->setMatrix3("normalMatrix", currentNormalMatrix);
		shader->setMatrix4("lightSpaceMatrix", currentLightSpaceMatrix);

		// Render mesh
		render(mesh.getIndiceCount());

		// Update diagnostics
		// Can be optimized with diagnostics update batch
		Diagnostics::addCurrentDrawCalls(1);
		Diagnostics::addCurrentVertices(static_cast<unsigned int>(mesh.getVerticeCount()));
		Diagnostics::addCurrentPolygons(static_cast<unsigned int>(mesh.getIndiceCount()) / 3);
	}
}

void MeshRenderer::prePass()
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	// Check if render target was culled this frame -> cancel
	if (isCulled())
		return;

	// Depth pre pass each mesh of entity
	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Set depth pre pass shader uniforms
		Shader* shader = Runtime::prePassShader;
		shader->setMatrix4("mvpMatrix", currentMvpMatrix);
		shader->setMatrix3("viewNormalMatrix", currentViewNormalMatrix);

		// Render mesh
		render(mesh.getIndiceCount());

		// Update diagnostics
		// Can be optimized with diagnostics update batch
		Diagnostics::addCurrentDrawCalls(1);
	}
}

void MeshRenderer::shadowPass()
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	// Skip shadow pass if model doesnt cast shadows
	if (!model->castsShadow)
		return;

	// Shadow pass each mesh of entity
	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Set shadow pass shader uniforms
		Shader* shader = Runtime::shadowPassShader;
		shader->setMatrix4("modelMatrix", currentModelMatrix);
		shader->setMatrix4("lightSpaceMatrix", currentLightSpaceMatrix);

		// Render mesh
		render(mesh.getIndiceCount());

		// Update diagnostics
		// Can be optimized with diagnostics update batch
		Diagnostics::addCurrentDrawCalls(1);
	}
}

void MeshRenderer::velocityPass()
{
	// No motion blur enabled -> cancel
	if (!useMotionBlur)
		return;

	// No model to render available -> cancel
	if (model == nullptr)
		return;

	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Set velocity pass shader uniforms
		Shader* shader = Runtime::velocityPassShader;
		shader->setMatrix4("modelMatrix", currentModelMatrix);
		shader->setMatrix4("previousModelMatrix", previousModelMatrix);
		shader->setMatrix4("viewMatrix", currentViewMatrix);
		shader->setMatrix4("projectionMatrix", currentProjectionMatrix);

		shader->setFloat("intensity", motionBlurIntensity);

		// Render mesh
		render(mesh.getIndiceCount());
	}

	// Cache mvp matrix
	previousModelMatrix = currentModelMatrix;
}

void MeshRenderer::render(unsigned int nElements)
{
	glDrawElements(GL_TRIANGLES, nElements, GL_UNSIGNED_INT, 0);
}

void MeshRenderer::performFrustumCulling()
{
	// No culling by default
	intersectsFrustum = true;

	// Update bounding volume
	volume->update(model, parentEntity->transform.position, parentEntity->transform.rotation, parentEntity->transform.scale);

	// Tmp, Add to cpu entities
	// Can be optimized with diagnostics update batch
	Diagnostics::addNEntitiesCPU(1);

	// Render target is not within frustum, cull it
	if (!volume->intersectsFrustum(Runtime::getCameraRendering().getFrustum()))
	{
		intersectsFrustum = false;
		return;
	}

	// Tmp, Add to gpu entities
	// Can be optimized with diagnostics update batch
	Diagnostics::addNEntitiesGPU(1);
}

bool MeshRenderer::isCulled()
{
	// Check if render target was culled
	return !intersectsFrustum;
}
