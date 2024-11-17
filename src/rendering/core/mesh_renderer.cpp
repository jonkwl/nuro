#include "mesh_renderer.h"

#include "../src/runtime/runtime.h"

glm::mat4 MeshRenderer::currentModelMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentViewMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentProjectionMatrix = glm::mat4(1.0);
glm::mat4 MeshRenderer::currentLightSpaceMatrix = glm::mat4(1.0);

MeshRenderer::MeshRenderer()
{
	this->model = nullptr;

	overwriteMaterials = false;
}

void MeshRenderer::forwardPass()
{
    // No model to render available -> cancel
    if (model == nullptr) return;

    // Calculate matrices
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

        // Mesh renderer doesnt contain custom overwrite materials, use models default materials 
        if (!overwriteMaterials) {
            // Try find material by models material index
            if (materialIndex < model->defaultMaterials.size()) {
                // Available material found -> use material
                material = model->defaultMaterials.at(materialIndex);
            }
        }
        // Mesh renderer contains custom materials
        else {
            // Try find material by models material index
            if (materialIndex < materials.size()) {
                // Available material found -> use material
                material = materials.at(materialIndex);
            }
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

    // Calculate model matrix
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
    // No model to render available -> cancel
    if (model == nullptr) return;

    // Skip shadow pass if model doesnt cast shadows
    if (!model->castsShadow) return;

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
