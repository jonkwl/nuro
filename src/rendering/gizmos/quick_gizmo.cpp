#include "quick_gizmo.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/rendering/model/model.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/model/mesh.h"

glm::vec3 QuickGizmo::color = glm::vec3(1.0f);

UnlitMaterial *QuickGizmo::material = nullptr;

Model *QuickGizmo::planeModel = nullptr;
Model *QuickGizmo::cubeModel = nullptr;
Model *QuickGizmo::sphereModel = nullptr;

std::vector<QuickGizmo::RenderTarget> QuickGizmo::renderStack;

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
    glGetIntegerv(GL_POLYGON_MODE, (GLint *)polygonState);

    // Bind gizmo material for upcoming renders
    material->bind();

    for (int i = 0; i < renderStack.size(); i++)
    {
        // Get gizmo rendering target
        RenderTarget gizmo = renderStack[i];

        // Calculate mvp
        glm::mat4 modelMatrix = getModelMatrix(gizmo.position, gizmo.rotation, gizmo.scale);
        glm::mat4 mvpMatrix = MeshRenderer::currentViewProjectionMatrix * modelMatrix;

        // Set material uniforms
        material->getShader()->setMatrix4("mvpMatrix", mvpMatrix);
        material->getShader()->setVec4("baseColor", glm::vec4(gizmo.color, 1.0f));

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
            Mesh *mesh = model->meshes[i];
            mesh->bind();
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        }
    }

    // Restore polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, polygonState[0]);
}

void QuickGizmo::plane(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = Shape::PLANE;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = false;
    renderStack.push_back(gizmo);
}

void QuickGizmo::box(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = Shape::BOX;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = false;
    renderStack.push_back(gizmo);
}

void QuickGizmo::sphere(glm::vec3 position, float radius)
{
    RenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = Shape::SPHERE;
    gizmo.position = position;
    gizmo.scale = glm::vec3(radius * 2);
    gizmo.rotation = glm::vec3(0.0f);
    gizmo.wireframe = false;
    renderStack.push_back(gizmo);
}

void QuickGizmo::planeWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = Shape::PLANE;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = true;
    renderStack.push_back(gizmo);
}

void QuickGizmo::boxWire(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    RenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = Shape::BOX;
    gizmo.position = position;
    gizmo.scale = scale;
    gizmo.rotation = rotation;
    gizmo.wireframe = true;
    renderStack.push_back(gizmo);
}

void QuickGizmo::sphereWire(glm::vec3 position, float radius)
{
    RenderTarget gizmo;
    gizmo.color = color;
    gizmo.shape = Shape::SPHERE;
    gizmo.position = position;
    gizmo.scale = glm::vec3(radius * 2);
    gizmo.rotation = glm::vec3(0.0f);
    gizmo.wireframe = true;
    renderStack.push_back(gizmo);
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
