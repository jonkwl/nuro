#include "entity_processor.h"

glm::mat4 EntityProcessor::currentView = glm::mat4(1.0);
glm::mat4 EntityProcessor::currentProjection = glm::mat4(1.0);
glm::mat4 EntityProcessor::currentLightSpace = glm::mat4(1.0);

IMaterial* EntityProcessor::defaultMaterial = nullptr;

Shader* EntityProcessor::shadowPassShader = nullptr;

EntityProcessor::EntityProcessor(Entity* entity)
{
	this->entity = entity;
}

void EntityProcessor::render()
{
    // No model to render available -> cancel
    if (entity->model == nullptr) return;

    // Calculate mvp
    glm::mat4 modelMatrix = Transformation::modelMatrix(entity);
    glm::mat4 mvp = currentProjection * currentView * modelMatrix;

    // Get model
    Model* model = entity->model;

    // Render each mesh of entity
    for (int i = 0; i < entity->model->meshes.size(); i++) {
        // Get current mesh
        Mesh* mesh = model->meshes[i];

        // Bind mesh
        mesh->bind();

        // Get mesh material by index
        IMaterial* material;

        // Try find material by models material index
        unsigned int materialIndex = mesh->getMaterialIndex();
        if (materialIndex < model->materials.size()) {

            // Available material found -> use material
            material = model->materials.at(materialIndex);

        }
        else {

            // No available material found -> use default material
            material = defaultMaterial;
        }

        // Bind material
        material->bind();

        // Set shader uniforms
        material->getShader()->setMatrix4("mvp", mvp);
        material->getShader()->setMatrix4("model", modelMatrix);
        material->getShader()->setMatrix4("lightSpace", currentLightSpace);

        // Render mesh
        mesh->render();
    }
}

void EntityProcessor::shadowPass()
{
    // No model to render available -> cancel
    if (entity->model == nullptr) return;

    // Calculate model matrix
    glm::mat4 modelMatrix = Transformation::modelMatrix(entity);

    // Get model
    Model* model = entity->model;

    // Shadow pass each mesh of entity
    for (int a = 0; a < model->meshes.size(); a++) {
        // Get current mesh
        Mesh* mesh = model->meshes.at(a);

        // Bind mesh
        mesh->bind();

        // Set shadow pass shader uniforms
        shadowPassShader->setMatrix4("model", modelMatrix);
        shadowPassShader->setMatrix4("lightSpace", currentLightSpace);

        // Render mesh
        mesh->render();
    }
}

void EntityProcessor::linkDefaults(IMaterial* _defaultMaterial, Shader* _shadowPassShader)
{
    defaultMaterial = _defaultMaterial;
    shadowPassShader = _shadowPassShader;
}
