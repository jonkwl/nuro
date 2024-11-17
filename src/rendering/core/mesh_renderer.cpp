#include "mesh_renderer.h"

#include "../src/runtime/runtime.h"
#include "../src/entity/entity.h"

glm::mat4 MeshRenderer::currentModelMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentViewMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentProjectionMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentLightSpaceMatrix = glm::mat4(1.0);

MeshRenderer::MeshRenderer(Entity* parentEntity)
{
    model = nullptr;
    volume = new BoundingSphere();

    this->parentEntity = parentEntity;
    intersectsFrustum = true;
}

void MeshRenderer::forwardPass()
{
    // No model to render available -> cancel
    if (model == nullptr) return;

    // Frustum culling
    performFrustumCulling();

    // Check if render target was culled this frame -> cancel
    if (isCulled()) return;

    // Calculate matrices
    currentModelMatrix = Transformation::modelMatrix(parentEntity);
    glm::mat4 mvpMatrix = currentProjectionMatrix * currentViewMatrix * currentModelMatrix;
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(currentModelMatrix));

    // Render each mesh of entity
    for (int i = 0; i < model->meshes.size(); i++) {
        // Get current mesh
        Mesh* mesh = model->meshes[i];

        // Bind mesh
        mesh->bind();

        // Get mesh material by index
        IMaterial* material = nullptr;

        unsigned int materialIndex = mesh->getMaterialIndex();
        // Try find material by models material index
        if (materialIndex < materials.size()) {
            // Available material found -> use material
            material = materials.at(materialIndex);
        }

        // No available material found -> use default material
        if (material == nullptr) {
            material = Runtime::defaultMaterial;
        }

        // Bind material
        material->bind();

        // Set shader uniforms
        material->getShader()->setMatrix4("mvpMatrix", mvpMatrix);
        material->getShader()->setMatrix4("modelMatrix", currentModelMatrix);
        material->getShader()->setMatrix3("normalMatrix", normalMatrix);
        material->getShader()->setMatrix4("lightSpaceMatrix", currentLightSpaceMatrix);

        // Render mesh
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

        // Update diagnostics
        Runtime::currentDrawCalls++;
        Runtime::currentVertices += mesh->vertices.size();
        Runtime::currentPolygons += mesh->indices.size() / 3;
    }
}

void MeshRenderer::prePass()
{
    // No model to render available -> cancel
    if (model == nullptr) return;

    // Perform frustum culling here

    // Calculate model and mvp matrix
    currentModelMatrix = Transformation::modelMatrix(parentEntity);
    glm::mat4 mvpMatrix = currentProjectionMatrix * currentViewMatrix * currentModelMatrix;

    // Depth pre pass each mesh of entity
    for (int a = 0; a < model->meshes.size(); a++) {
        // Get current mesh
        Mesh* mesh = model->meshes.at(a);

        // Bind mesh
        mesh->bind();

        // Set depth pre pass shader uniforms
        Shader* depthPrePassShader = Runtime::prePassShader;
        depthPrePassShader->setMatrix4("mvp", mvpMatrix);

        // Render mesh
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

        // Update diagnostics
        Runtime::currentDrawCalls++;
    }
}

void MeshRenderer::shadowPass()
{
    // Check if render target was culled this frame -> cancel
    if (isCulled()) return;

    // No model to render available -> cancel
    if (model == nullptr) return;

    // Skip shadow pass if model doesnt cast shadows
    if (!model->castsShadow) return;

    // Calculate current model matrix
    currentModelMatrix = Transformation::modelMatrix(parentEntity);

    // Shadow pass each mesh of entity
    for (int a = 0; a < model->meshes.size(); a++) {
        // Get current mesh
        Mesh* mesh = model->meshes.at(a);

        // Bind mesh
        mesh->bind();

        // Set shadow pass shader uniforms
        Shader* shadowPassShader = Runtime::shadowPassShader;
        shadowPassShader->setMatrix4("modelMatrix", currentModelMatrix);
        shadowPassShader->setMatrix4("lightSpaceMatrix", currentLightSpaceMatrix);

        // Render mesh
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

        // Update diagnostics
        Runtime::currentDrawCalls++;
    }
}

void MeshRenderer::performFrustumCulling()
{
    // No culling by default
    intersectsFrustum = true;
    
    // Update bounding volume
    volume->update(model, parentEntity->transform.position, parentEntity->transform.rotation, parentEntity->transform.scale);

    // Tmp, Add to cpu entities
    Runtime::nCPUEntities++;

    // Render target is not within frustum, cull it
    if (!volume->intersectsFrustum(Runtime::getCameraRendering()->frustum)) {
        intersectsFrustum = false;
        return;
    }

    // Tmp, Add to gpu entities
    Runtime::nGPUEntities++;
}

bool MeshRenderer::isCulled()
{
    // Check if render target was culled
    return !intersectsFrustum;
}
