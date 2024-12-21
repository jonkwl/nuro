#include "imgizmo.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "../core/rendering/model/model.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/model/mesh.h"
#include "../core/utils/log.h"

// Global gizmo resources
Shader* IMGizmo::fillShader = nullptr;
Shader* IMGizmo::iconShader = nullptr;
Model* IMGizmo::planeModel = nullptr;
Model* IMGizmo::cubeModel = nullptr;
Model* IMGizmo::sphereModel = nullptr;

IMGizmo::IMGizmo() : color(glm::vec3(1.0f)),
opacity(0.4f),
foreground(false),
shapeRenderStack(),
iconRenderStack()
{
}

void IMGizmo::setup()
{
	// Load global gizmo shaders and models if not loaded already
	if (!fillShader) fillShader = ShaderPool::get("gizmo_fill");
	if (!iconShader) iconShader = ShaderPool::get("gizmo_icon");
	if(!planeModel) planeModel = Model::load("../resources/primitives/plane.fbx");
	if (!cubeModel) cubeModel = Model::load("../resources/primitives/cube.fbx");
	if(!sphereModel) sphereModel = Model::load("../resources/primitives/sphere.fbx");
}

void IMGizmo::newFrame()
{
	// Clear render stacks
	shapeRenderStack.clear();
	iconRenderStack.clear();
}

void IMGizmo::render(const glm::mat4& viewProjection)
{
	renderShapes(viewProjection);
	renderIcons(viewProjection);
}

void IMGizmo::plane(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
	ShapeRenderTarget gizmo(Shape::PLANE, position, rotation, scale, false, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::box(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
	ShapeRenderTarget gizmo(Shape::BOX, position, rotation, scale, false, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::sphere(glm::vec3 position, float radius)
{
	ShapeRenderTarget gizmo(Shape::SPHERE, position, glm::vec3(0.0f), glm::vec3(radius * 2), false, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::planeWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
	ShapeRenderTarget gizmo(Shape::PLANE, position, rotation, scale, true, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::boxWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
	ShapeRenderTarget gizmo(Shape::BOX, position, rotation, scale, true, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::sphereWire(glm::vec3 position, float radius)
{
	ShapeRenderTarget gizmo(Shape::SPHERE, position, glm::vec3(0.0f), glm::vec3(radius * 2), true, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::icon3d(Texture& icon, glm::vec3 position, TransformComponent& cameraTransform, glm::vec3 scale)
{
	IconRenderTarget gizmo(icon, position, scale, cameraTransform, getCurrentState());
	iconRenderStack.push_back(gizmo);
}

Model* IMGizmo::getCubeModel()
{
	return cubeModel;
}

void IMGizmo::renderShapes(const glm::mat4& viewProjection)
{
	// Cache current polygon mode
	GLenum polygonState[2];
	glGetIntegerv(GL_POLYGON_MODE, (GLint*)polygonState);

	// Bind quick gizmo shader for upcoming renders
	fillShader->bind();

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int32_t i = 0; i < shapeRenderStack.size(); i++)
	{
		// Get gizmo rendering target
		ShapeRenderTarget gizmo = shapeRenderStack[i];

		// Disable depth testing if gizmo should be in foreground
		if (gizmo.state.foreground) {
			glDisable(GL_DEPTH_TEST);
		}

		// Calculate mvp
		glm::mat4 modelMatrix = getModelMatrix(gizmo.position, gizmo.rotation, gizmo.scale);
		glm::mat4 mvpMatrix = viewProjection * modelMatrix;

		// Set material uniforms
		fillShader->setMatrix4("mvpMatrix", mvpMatrix);
		fillShader->setVec4("color", glm::vec4(gizmo.state.color, gizmo.state.opacity));

		// Get model
		Model* model = getModel(gizmo.shape);

		// Set polygon mode for gizmo render
		if (gizmo.wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Render each mesh
		for (int32_t i = 0; i < model->meshes.size(); i++)
		{
			Mesh& mesh = model->meshes[i];
			glBindVertexArray(mesh.getVAO());
			glDrawElements(GL_TRIANGLES, mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);
		}

		// Re-Enable depth testing if gizmo was in foreground
		if (gizmo.state.foreground) {
			glEnable(GL_DEPTH_TEST);
		}
	}

	// Disable blending
	glDisable(GL_BLEND);

	// Restore polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, polygonState[0]);
}

void IMGizmo::renderIcons(const glm::mat4& viewProjection)
{
	// Bind quick gizmo shader for upcoming renders
	iconShader->bind();

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Fill polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (int32_t i = 0; i < iconRenderStack.size(); i++)
	{
		// Get gizmo rendering target
		IconRenderTarget gizmo = iconRenderStack[i];

		// Bind gizmo icon texture
		gizmo.icon.bind(0);

		// Get cameras position and direction
		glm::vec3 gizmoPosition = Transformation::toBackendPosition(gizmo.position);
		glm::vec3 cameraPosition = Transformation::toBackendPosition(gizmo.cameraTransform.position);
		// Compute direction vector from gizmo to camera in XZ plane
		glm::vec3 directionToCamera = glm::normalize(glm::vec3(cameraPosition.x - gizmoPosition.x, 0.0f, cameraPosition.z - gizmoPosition.z));
		glm::vec3 right = glm::normalize(glm::vec3(-directionToCamera.z, 0.0f, directionToCamera.x));
		glm::vec3 forward = glm::vec3(directionToCamera.x, 0.0f, directionToCamera.z);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // World up
		// Construct model matrix for gizmo
		glm::mat4 modelMatrix = glm::mat4(
			glm::vec4(right, 0.0f),
			glm::vec4(up, 0.0f),
			glm::vec4(forward, 0.0f),
			glm::vec4(gizmoPosition, 1.0f)
		);
		// Calculate and apply 180-degree rotation around Z-axis
		glm::mat4 rotation180Z = glm::mat4(
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		modelMatrix = modelMatrix * rotation180Z;
		// Scale gizmo icon
		modelMatrix = modelMatrix * glm::scale(glm::mat4(1.0f), gizmo.scale);
		// Calculate MVP matrix
		glm::mat4 mvpMatrix = viewProjection * modelMatrix;

		// Set material uniforms
		iconShader->setMatrix4("mvpMatrix", mvpMatrix);
		iconShader->setVec4("color", glm::vec4(gizmo.state.color, gizmo.state.opacity));
		iconShader->setVec3("tint", glm::vec3(1.0f));
		iconShader->setFloat("alpha", 0.35f);

		// Get model
		Model* model = getModel(Shape::PLANE);

		// Render each mesh
		for (int32_t i = 0; i < model->meshes.size(); i++)
		{
			Mesh& mesh = model->meshes[i];
			glBindVertexArray(mesh.getVAO());
			glDrawElements(GL_TRIANGLES, mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);
		}
	}

	// Disable blending
	glDisable(GL_BLEND);
}

IMGizmo::RenderState IMGizmo::getCurrentState() {
	RenderState state;
	state.color = color;
	state.opacity = opacity;
	state.foreground = foreground;
	return state;
}

Model* IMGizmo::getModel(Shape shape)
{
	switch (shape)
	{
	case Shape::PLANE:
		return planeModel;
	case Shape::BOX:
		return cubeModel;
		break;
	case Shape::SPHERE:
		return sphereModel;
		break;
	}
}

glm::mat4 IMGizmo::getModelMatrix(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
{
	glm::mat4 model(1.0f);

	// object position
	glm::vec3 worldPosition = Transformation::toBackendPosition(position);
	model = glm::translate(model, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

	// object rotation
	rotation = glm::normalize(rotation);
	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
	model = model * rotationMatrix;

	// object scale
	model = glm::scale(model, scale);

	return model;
}

glm::mat4 IMGizmo::getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glm::mat4 model(1.0f);

	// object position
	glm::vec3 worldPosition = Transformation::toBackendPosition(position);
	model = glm::translate(model, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

	// object rotation
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	// object scale
	model = glm::scale(model, scale);

	return model;
}