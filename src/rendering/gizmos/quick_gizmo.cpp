#include "quick_gizmo.h"

#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/core/transformation.h"

// Initialize static gizmo colors

const glm::vec3 GizmoColor::RED = { 1.0f, 0.0f, 0.0f };
const glm::vec3 GizmoColor::GREEN = { 0.0f, 1.0f, 0.0f };
const glm::vec3 GizmoColor::BLUE = { 0.0f, 0.0f, 1.0f };
const glm::vec3 GizmoColor::YELLOW = { 1.0f, 1.0f, 0.0f };
const glm::vec3 GizmoColor::CYAN = { 0.0f, 1.0f, 1.0f };
const glm::vec3 GizmoColor::MAGENTA = { 1.0f, 0.0f, 1.0f };
const glm::vec3 GizmoColor::WHITE = { 1.0f, 1.0f, 1.0f };
const glm::vec3 GizmoColor::BLACK = { 0.0f, 0.0f, 0.0f };
const glm::vec3 GizmoColor::GRAY = { 0.5f, 0.5f, 0.5f };
const glm::vec3 GizmoColor::DARK_GRAY = { 0.25f, 0.25f, 0.25f };
const glm::vec3 GizmoColor::LIGHT_GRAY = { 0.75f, 0.75f, 0.75f };
const glm::vec3 GizmoColor::ORANGE = { 1.0f, 0.5f, 0.0f };
const glm::vec3 GizmoColor::PURPLE = { 0.5f, 0.0f, 0.5f };
const glm::vec3 GizmoColor::BROWN = { 0.6f, 0.3f, 0.1f };
const glm::vec3 GizmoColor::PINK = { 1.0f, 0.75f, 0.8f };
const glm::vec3 GizmoColor::DARK_RED = { 0.5f, 0.0f, 0.0f };
const glm::vec3 GizmoColor::DARK_GREEN = { 0.0f, 0.5f, 0.0f };
const glm::vec3 GizmoColor::DARK_BLUE = { 0.0f, 0.0f, 0.5f };
const glm::vec3 GizmoColor::DARK_YELLOW = { 0.5f, 0.5f, 0.0f };
const glm::vec3 GizmoColor::DARK_CYAN = { 0.0f, 0.5f, 0.5f };
const glm::vec3 GizmoColor::DARK_MAGENTA = { 0.5f, 0.0f, 0.5f };
const glm::vec3 GizmoColor::DARK_ORANGE = { 0.5f, 0.25f, 0.0f };
const glm::vec3 GizmoColor::DARK_PURPLE = { 0.25f, 0.0f, 0.25f };
const glm::vec3 GizmoColor::DARK_BROWN = { 0.3f, 0.15f, 0.05f };
const glm::vec3 GizmoColor::DARK_PINK = { 0.5f, 0.375f, 0.4f };

// Initialize quick gizmo members

glm::vec3 QuickGizmo::color = glm::vec3(1.0f);

UnlitMaterial* QuickGizmo::material = nullptr;

Model* QuickGizmo::planeModel = nullptr;
Model* QuickGizmo::cubeModel = nullptr;
Model* QuickGizmo::sphereModel = nullptr;

std::vector<GizmoRenderTarget> QuickGizmo::renderStack;

void QuickGizmo::setup()
{
	// Setup material
	material = new UnlitMaterial();
    
    // Load models from primitives
    planeModel = new Model("./resources/primitives/plane.fbx");
    cubeModel = new Model("./resources/primitives/cube.fbx");
    sphereModel = new Model("./resources/primitives/sphere.fbx");
}

void QuickGizmo::newFrame()
{
    // Clear render stack
    renderStack.clear();
}

void QuickGizmo::render()
{
    // Cache current polygon mode
    GLenum polygonState[2];
    glGetIntegerv(GL_POLYGON_MODE, (GLint*)polygonState);

    // Bind gizmo material for upcoming renders
    material->bind();

    for (int i = 0; i < renderStack.size(); i++) {
        // Get gizmo rendering target
        GizmoRenderTarget gizmo = renderStack[i];

        // Calculate mvp
        glm::mat4 modelMatrix = getModelMatrix(gizmo.position, gizmo.rotation, gizmo.scale);
        glm::mat4 mvpMatrix = MeshRenderer::currentViewProjectionMatrix * modelMatrix;

        // Set material uniforms
        material->getShader()->setMatrix4("mvpMatrix", mvpMatrix);
        material->getShader()->setVec4("baseColor", glm::vec4(gizmo.color, 1.0f));

        // Get model
        Model* model = getModel(gizmo.shape);

        // Set polygon mode for gizmo render
        if (gizmo.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Render each mesh
        for (int i = 0; i < model->meshes.size(); i++) {
            Mesh* mesh = model->meshes[i];
            mesh->bind();
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        }
    }

    // Restore polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, polygonState[0]);
}

void QuickGizmo::plane(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    GizmoRenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = GizmoShape::PLANE;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = false;
    renderStack.push_back(gizmo);
}

void QuickGizmo::box(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    GizmoRenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = GizmoShape::BOX;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = false;
    renderStack.push_back(gizmo);
}

void QuickGizmo::sphere(glm::vec3 position, float radius) {
    GizmoRenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = GizmoShape::SPHERE;
    gizmo.position = position;
    gizmo.scale = glm::vec3(radius * 2);
    gizmo.rotation = glm::vec3(0.0f);
    gizmo.wireframe = false;
    renderStack.push_back(gizmo);
}

void QuickGizmo::planeWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    GizmoRenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = GizmoShape::PLANE;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = true;
    renderStack.push_back(gizmo);
}

void QuickGizmo::boxWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    GizmoRenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = GizmoShape::BOX;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = true;
    renderStack.push_back(gizmo);
}

void QuickGizmo::sphereWire(glm::vec3 position, float radius) {
    GizmoRenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = GizmoShape::SPHERE;
    gizmo.position = position;
    gizmo.scale = glm::vec3(radius * 2);
    gizmo.rotation = glm::vec3(0.0f);
    gizmo.wireframe = true;
    renderStack.push_back(gizmo);
}

Model* QuickGizmo::getModel(GizmoShape shape)
{
    switch (shape) {
    case PLANE:
        return planeModel;
    case BOX:
        return cubeModel;
        break;
    case SPHERE:
        return sphereModel;
        break;
    }
}

glm::mat4 QuickGizmo::getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    glm::mat4 model(1.0f);

    // object position
    glm::vec3 worldPosition = Transformation::prepareWorldPosition(position);
    model = glm::translate(model, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

    // object rotation
    glm::vec3 worldRotation = Transformation::prepareWorldRotation(rotation);
    model = glm::rotate(model, glm::radians(worldRotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(worldRotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(worldRotation.z), glm::vec3(0, 0, 1));

    // object scale
    model = glm::scale(model, scale);

    return model;
}
