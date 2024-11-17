#include "mesh_renderer.h"

MeshRenderer::MeshRenderer(Model* model)
{
	this->model = model;
	overwriteMaterials = false;
}
