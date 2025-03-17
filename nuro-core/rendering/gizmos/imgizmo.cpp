#include "imgizmo.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utils/console.h>
#include <rendering/model/mesh.h>
#include <rendering/model/model.h>
#include <rendering/shader/shader.h>
#include <context/application_context.h>
#include <rendering/shader/shader_pool.h>
#include <rendering/transformation/transformation.h>

// Global gizmo resources
IMGizmo::StaticData IMGizmo::staticData;

IMGizmo::IMGizmo() : color(glm::vec3(1.0f)),
opacity(0.4f),
foreground(false),
iconScale(0.8f),
shapeRenderStack(),
iconRenderStack()
{
}

void IMGizmo::create()
{
	// Load all static data if not loaded already
	if (!staticData.loaded) {

		ResourceManager& resource = ApplicationContext::resourceManager();

		auto[planeId, plane] = resource.create<Model>();
		plane->setSource("resources/primitives/plane.fbx");
		resource.loadAsync(plane);

		auto [sphereId, sphere] = resource.create<Model>();
		sphere->setSource("resources/primitives/sphere.fbx");
		resource.loadAsync(sphere);

		staticData.fillShader = ShaderPool::get("gizmo_fill");
		staticData.iconShader = ShaderPool::get("gizmo_icon");

		staticData.planeMesh = plane->queryMesh(0);
		staticData.boxMesh = createBoxMesh();
		staticData.sphereMesh = sphere->queryMesh(0);
	}
}

void IMGizmo::newFrame()
{
	// Clear render stacks
	shapeRenderStack.clear();
	iconRenderStack.clear();
}

void IMGizmo::renderAll(const glm::mat4& viewProjection)
{
	renderShapes(viewProjection);
	renderIcons(viewProjection);
}

void IMGizmo::renderShapes(const glm::mat4& viewProjection)
{
	// Cache current polygon mode
	GLenum polygonState[2];
	glGetIntegerv(GL_POLYGON_MODE, (GLint*)polygonState);

	// Bind quick gizmo shader for upcoming renders
	staticData.fillShader->bind();

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	for (int32_t i = 0; i < shapeRenderStack.size(); i++)
	{
		// Get gizmo rendering target
		ShapeRenderTarget gizmo = shapeRenderStack[i];

		// Calculate mvp
		glm::mat4 modelMatrix = getModelMatrix(gizmo.position, gizmo.rotation, gizmo.scale);
		glm::mat4 mvpMatrix = viewProjection * modelMatrix;

		// Set material uniforms
		staticData.fillShader->setMatrix4("mvpMatrix", mvpMatrix);
		staticData.fillShader->setVec4("color", glm::vec4(gizmo.state.color, gizmo.state.opacity));

		// Set polygon mode for gizmo render
		if (gizmo.wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Render mesh
		const Mesh* mesh = queryMesh(gizmo.shape);
		glBindVertexArray(mesh->vao());
		glDrawElements(GL_LINES, mesh->indiceCount(), GL_UNSIGNED_INT, 0);

		// Optional foreground pass without depth testing and reduced opacity
		if (gizmo.state.foreground) {
			staticData.fillShader->setVec4("color", glm::vec4(gizmo.state.color, 0.035f));
			glDisable(GL_DEPTH_TEST);
			glDrawElements(GL_LINES, mesh->indiceCount(), GL_UNSIGNED_INT, 0);
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
	// Setup
	const float renderRadius = 50.0f;

	// Bind quick gizmo shader for upcoming renders
	staticData.iconShader->bind();

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Fill polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	for (int32_t i = 0; i < iconRenderStack.size(); i++)
	{
		//
		// UNOPTIMIZED CODE!
		//

		// Get gizmo rendering target
		IconRenderTarget gizmo = iconRenderStack[i];

		// Dont render gizmo if exceeding render radius
		if (glm::distance(gizmo.position, gizmo.cameraTransform.position) > renderRadius) continue;

		// Bind gizmo icon texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gizmo.iconTexture);

		// Get cameras position and direction
		glm::vec3 gizmoPosition = Transformation::swap(gizmo.position);
		glm::vec3 cameraPosition = Transformation::swap(gizmo.cameraTransform.position);
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
		modelMatrix = modelMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(gizmo.scale));
		// Calculate MVP matrix
		glm::mat4 mvpMatrix = viewProjection * modelMatrix;

		// Get mesh
		const Mesh* mesh = queryMesh(Shape::PLANE);

		// Calculates and returns the icon alpha relative to the camera
		auto alpha = [](float baseAlpha, glm::vec3 iconPosition, glm::vec3 cameraPosition) {
			float value = baseAlpha;
			float minDistance = 0.5f;
			float maxDistance = 2.5f;
			float distance = glm::distance(iconPosition, cameraPosition);
			if (distance < maxDistance) {
				value = glm::mix(0.0f, value, glm::smoothstep(minDistance, maxDistance, distance));
			}
			return value;
		};

		// Set static material uniforms
		staticData.iconShader->setMatrix4("mvpMatrix", mvpMatrix);
		staticData.iconShader->setVec3("tint", glm::vec3(1.0f));

		// Render with full opacity and depth test
		staticData.iconShader->setFloat("alpha", alpha(1.0f, gizmoPosition, cameraPosition));
		glBindVertexArray(mesh->vao());
		glDrawElements(GL_TRIANGLES, mesh->indiceCount(), GL_UNSIGNED_INT, 0);

		// Render with transparency but without depth test
		staticData.iconShader->setFloat("alpha", alpha(0.06f, gizmoPosition, cameraPosition));
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, mesh->indiceCount(), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
	}

	// Disable blending
	glDisable(GL_BLEND);
}

void IMGizmo::plane(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation)
{
	ShapeRenderTarget gizmo(Shape::PLANE, position, rotation, scale, false, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::box(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation)
{
	ShapeRenderTarget gizmo(Shape::BOX, position, rotation, scale, false, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::sphere(const glm::vec3& position, float radius, const glm::quat& rotation)
{
	ShapeRenderTarget gizmo(Shape::SPHERE, position, rotation, glm::vec3(radius * 2), false, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::planeWire(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation)
{
	ShapeRenderTarget gizmo(Shape::PLANE, position, rotation, scale, true, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::boxWire(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation)
{
	ShapeRenderTarget gizmo(Shape::BOX, position, rotation, scale, true, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::sphereWire(const glm::vec3& position, float radius, const glm::quat& rotation)
{
	ShapeRenderTarget gizmo(Shape::SPHERE, position, rotation, glm::vec3(radius), true, getCurrentState());
	shapeRenderStack.push_back(gizmo);
}

void IMGizmo::icon3d(uint32_t iconTexture, const glm::vec3& position, TransformComponent& cameraTransform)
{
	IconRenderTarget gizmo(iconTexture, position, iconScale, cameraTransform, getCurrentState());
	iconRenderStack.push_back(gizmo);
}

IMGizmo::RenderState IMGizmo::getCurrentState() {
	RenderState state;
	state.color = color;
	state.opacity = opacity;
	state.foreground = foreground;
	return state;
}

const Mesh* IMGizmo::queryMesh(Shape shape)
{
	switch (shape)
	{
	case Shape::PLANE:
		return staticData.planeMesh;
	case Shape::BOX:
		return staticData.boxMesh;
		break;
	case Shape::SPHERE:
		return staticData.sphereMesh;
		break;
	default:
		return staticData.boxMesh;
		break;
	}
}

glm::mat4 IMGizmo::getModelMatrix(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
{
	glm::mat4 model(1.0f);

	// object position
	glm::vec3 worldPosition = Transformation::swap(position);
	model = glm::translate(model, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

	// object rotation
	rotation = Transformation::swap(rotation);
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
	glm::vec3 worldPosition = Transformation::swap(position);
	model = glm::translate(model, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

	// object rotation
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	// object scale
	model = glm::scale(model, scale);

	return model;
}

const Mesh* IMGizmo::createPlaneMesh()
{
	const glm::vec2 zeroVec2 = glm::vec2(0.0f);
	const glm::vec3 zeroVec3 = glm::vec3(0.0f);

	// Vertices of the plane
	std::vector<Model::VertexData> vertices = {
		Model::VertexData(glm::vec3(-1.0f, 0.0f, -1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3), // 0
		Model::VertexData(glm::vec3(1.0f, 0.0f, -1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),  // 1
		Model::VertexData(glm::vec3(1.0f, 0.0f, 1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),   // 2
		Model::VertexData(glm::vec3(-1.0f, 0.0f, 1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3)   // 3
	};

	// Indices of the planes face
	std::vector<uint32_t> indices = {
		0, 1, 1, 2, 2, 3, 3, 0
	};

	// Create and return mesh
	return Model::createStaticMesh(vertices, indices);
}

const Mesh* IMGizmo::createBoxMesh()
{
	const glm::vec2 zeroVec2 = glm::vec2(0.0f);
	const glm::vec3 zeroVec3 = glm::vec3(0.0f);

	// Vertices of the box
	std::vector<Model::VertexData> vertices = {
		Model::VertexData(glm::vec3(-1.0f, -1.0f, -1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),  // 0
		Model::VertexData(glm::vec3(1.0f, -1.0f, -1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),   // 1
		Model::VertexData(glm::vec3(1.0f,  1.0f, -1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),   // 2
		Model::VertexData(glm::vec3(-1.0f,  1.0f, -1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),  // 3
		Model::VertexData(glm::vec3(-1.0f, -1.0f,  1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),  // 4
		Model::VertexData(glm::vec3(1.0f, -1.0f,  1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),   // 5
		Model::VertexData(glm::vec3(1.0f,  1.0f,  1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3),   // 6
		Model::VertexData(glm::vec3(-1.0f,  1.0f,  1.0f), zeroVec3, zeroVec2, zeroVec3, zeroVec3)   // 7
	};

	// Indices of the boxes faces
	std::vector<uint32_t> indices = {
		0, 1, 1, 2, 2, 3, 3, 0, // Bottom face (edges)
		4, 5, 5, 6, 6, 7, 7, 4, // Top face (edges)
		0, 4, 1, 5, 2, 6, 3, 7  // Side edges
	};

	// Create and return mesh
	return Model::createStaticMesh(vertices, indices);
}