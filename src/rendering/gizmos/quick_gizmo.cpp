#include "quick_gizmo.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "../src/rendering/model/model.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/model/mesh.h"


glm::vec3 QuickGizmo::color = glm::vec3(1.0f);
float QuickGizmo::opacity = 0.4f;
bool QuickGizmo::foreground = false;

Shader *QuickGizmo::shader = nullptr;

Model *QuickGizmo::planeModel = nullptr;
Model *QuickGizmo::cubeModel = nullptr;
Model *QuickGizmo::sphereModel = nullptr;

std::vector<QuickGizmo::RenderTarget> QuickGizmo::renderStack;

void QuickGizmo::setup()
{
    // Setup material
    shader = ShaderPool::get("quick_gizmo");

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
    glGetIntegerv(GL_POLYGON_MODE, (GLint *)polygonState);

    // Bind quick gizmo shader for upcoming renders
    shader->bind();

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < renderStack.size(); i++)
    {
        // Get gizmo rendering target
        RenderTarget gizmo = renderStack[i];

        // Disable depth testing if gizmo should be in foreground
        if (gizmo.state.foreground) {
            glDisable(GL_DEPTH_TEST);
        }

        // Calculate mvp
        glm::mat4 modelMatrix = getModelMatrix(gizmo.position, gizmo.rotation, gizmo.scale);
        glm::mat4 mvpMatrix = MeshRenderer::currentViewProjectionMatrix * modelMatrix;

        // Set material uniforms
        shader->setMatrix4("mvpMatrix", mvpMatrix);
        shader->setVec4("color", glm::vec4(gizmo.state.color, gizmo.state.opacity));

        // Get model
        Model *model = getModel(gizmo.shape);

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
        for (int i = 0; i < model->meshes.size(); i++)
        {
            Mesh mesh = model->meshes[i];
            mesh.bind();
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

void QuickGizmo::plane(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo(Shape::PLANE, position, rotation, scale, false);
    renderStack.push_back(gizmo);
}

void QuickGizmo::box(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo(Shape::BOX, position, rotation, scale, false);
    renderStack.push_back(gizmo);
}

void QuickGizmo::sphere(glm::vec3 position, float radius)
{
    RenderTarget gizmo(Shape::SPHERE, position, glm::vec3(radius * 2), glm::vec3(0.0f), false);
    renderStack.push_back(gizmo);
}

void QuickGizmo::planeWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo(Shape::PLANE, position, rotation, scale, true);
    renderStack.push_back(gizmo);
}

void QuickGizmo::boxWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo(Shape::BOX, position, rotation, scale, true);
    renderStack.push_back(gizmo);
}

void QuickGizmo::sphereWire(glm::vec3 position, float radius)
{
    RenderTarget gizmo(Shape::SPHERE, position, glm::vec3(radius * 2), glm::vec3(0.0f), true);
    renderStack.push_back(gizmo);
}

QuickGizmo::RenderState QuickGizmo::getCurrentState() {
    RenderState state;
    state.color = color;
    state.opacity = opacity;
    state.foreground = foreground;
    return state;
}

Model *QuickGizmo::getModel(Shape shape)
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
