#include "mesh_renderer.h"

#include <glad/glad.h>

#include "../core/rendering/model/model.h"
#include "../core/rendering/model/mesh.h"
#include "../core/rendering/culling/bounding_volume.h"
#include "../core/old_entity/old_entity.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/material/unlit/unlit_material.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/rendering/shader/shader_pool.h"

MeshRenderer::MeshRenderer(OldEntity* parentEntity) : parentEntity(parentEntity),
model(nullptr),
volume(new BoundingSphere()),
useMotionBlur(false),
motionBlurIntensity(1.0f),
currentModelMatrix(glm::mat4(1.0f)),
currentMvpMatrix(glm::mat4(1.0f)),
currentNormalMatrix(glm::mat4(1.0f)),
previousModelMatrix(glm::mat4(1.0f)),
materialUnavailableShader(ShaderPool::get("mat_unavailable"))
{
}

void MeshRenderer::prepareNextFrame()
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	glm::mat4 currentViewProjectionMatrix = glm::mat4(1.0f);

	// Calculate and cache model and mvp matrix for current frame
	currentModelMatrix = Transformation::model(parentEntity->transform);
	currentMvpMatrix = currentViewProjectionMatrix * currentModelMatrix;
	currentNormalMatrix = glm::transpose(glm::inverse(currentModelMatrix));

	// Frustum culling
	//
}

void MeshRenderer::forwardPass()
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	glm::mat4 currentLightSpaceMatrix = glm::mat4(1.0f);

	// Render each mesh of entity
	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Material selection can be optimized: No need to select material each frame

		// Get mesh material by index
		IMaterial* material = nullptr;

		unsigned int materialIndex = mesh.getMaterialIndex();
		// Try find material by models material index
		if (materialIndex < materials.size())
		{
			// Available material found -> use material
			material = materials[materialIndex];
		}

		// Available material found
		if (material)
		{

			// Bind material
			material->bind();

			// Set shader uniforms
			Shader* shader = material->getShader();
			shader->setMatrix4("mvpMatrix", currentMvpMatrix);
			shader->setMatrix4("modelMatrix", currentModelMatrix);
			shader->setMatrix3("normalMatrix", currentNormalMatrix);
			shader->setMatrix4("lightSpaceMatrix", currentLightSpaceMatrix);

		}
		// No available material found
		else {

			// Use unavailable material shader 
			Shader* shader = materialUnavailableShader;
			shader->bind();
			shader->setMatrix4("mvpMatrix", currentMvpMatrix);

		}

		// Render mesh
		render(mesh.getIndiceCount());

		// Update diagnostics
		// Can be optimized with diagnostics update batch
		Diagnostics::addCurrentDrawCalls(1);
		Diagnostics::addCurrentVertices(static_cast<unsigned int>(mesh.getVerticeCount()));
		Diagnostics::addCurrentPolygons(static_cast<unsigned int>(mesh.getIndiceCount()) / 3);
	}
}

void MeshRenderer::prePass(Shader* shader)
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	glm::mat4 currentViewNormalMatrix = glm::mat4(1.0f);

	// Depth pre pass each mesh of entity
	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Set depth pre pass shader uniforms
		shader->setMatrix4("mvpMatrix", currentMvpMatrix);
		shader->setMatrix3("viewNormalMatrix", currentViewNormalMatrix);

		// Render mesh
		render(mesh.getIndiceCount());

		// Update diagnostics
		// Can be optimized with diagnostics update batch
		Diagnostics::addCurrentDrawCalls(1);
	}
}

void MeshRenderer::shadowPass(Shader* shader)
{
	// No model to render available -> cancel
	if (model == nullptr)
		return;

	// Skip shadow pass if model doesnt cast shadows
	if (!model->castsShadow)
		return;

	glm::mat4 currentLightSpaceMatrix = glm::mat4(1.0f);

	// Shadow pass each mesh of entity
	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Set shadow pass shader uniforms
		shader->setMatrix4("modelMatrix", currentModelMatrix);
		shader->setMatrix4("lightSpaceMatrix", currentLightSpaceMatrix);

		// Render mesh
		render(mesh.getIndiceCount());

		// Update diagnostics
		// Can be optimized with diagnostics update batch
		Diagnostics::addCurrentDrawCalls(1);
	}
}

void MeshRenderer::velocityPass(Shader* shader)
{
	// No motion blur enabled -> cancel
	if (!useMotionBlur)
		return;

	// No model to render available -> cancel
	if (model == nullptr)
		return;

	glm::mat4 currentViewMatrix = glm::mat4(1.0f);
	glm::mat4 currentProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 currentViewProjectionMatrix = glm::mat4(1.0f);

	for (int i = 0; i < model->meshes.size(); i++)
	{
		// Get current mesh
		Mesh mesh = model->meshes[i];

		// Bind mesh
		mesh.bind();

		// Set velocity pass shader uniforms
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

void MeshRenderer::recalculateRenderMatrices()
{
	glm::mat4 currentViewProjectionMatrix = glm::mat4(1.0f);

	currentModelMatrix = Transformation::model(parentEntity->transform);
	currentMvpMatrix = currentViewProjectionMatrix * currentModelMatrix;
	currentNormalMatrix = glm::transpose(glm::inverse(currentModelMatrix));
}

void MeshRenderer::render(unsigned int nElements)
{
	glDrawElements(GL_TRIANGLES, nElements, GL_UNSIGNED_INT, 0);
}