#include "shapes.h"

#include <cmath>
#include <vector>
#include <glm/glm.hpp>

#include <rendering/model/mesh.h>
#include <rendering/model/model.h>

namespace Shapes {

	Mesh* createPlane() {
        std::vector<Model::VertexData> vertices = {
        Model::VertexData(glm::vec3(-1, 0, -1), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
        Model::VertexData(glm::vec3(1, 0, -1), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
        Model::VertexData(glm::vec3(-1, 0, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
        Model::VertexData(glm::vec3(1, 0, 1), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1))
        };

        std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 1, 3
        };

        return Model::createStaticMesh(vertices, indices);
	}

	Mesh* createCube() {
        std::vector<Model::VertexData> vertices = {
            // Front Face
            Model::VertexData(glm::vec3(-1, -1,  1), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(1, -1,  1), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(1,  1,  1), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(-1,  1,  1), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),

            // Back Face
            Model::VertexData(glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(-1,  1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(1,  1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)),

            // Left Face
            Model::VertexData(glm::vec3(-1, -1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(-1, -1,  1), glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(-1,  1,  1), glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(-1,  1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),

            // Right Face
            Model::VertexData(glm::vec3(1, -1,  1), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(1,  1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            Model::VertexData(glm::vec3(1,  1,  1), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),

            // Bottom Face
            Model::VertexData(glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
            Model::VertexData(glm::vec3(1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
            Model::VertexData(glm::vec3(1, -1,  1), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),
            Model::VertexData(glm::vec3(-1, -1,  1), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)),

            // Top Face
            Model::VertexData(glm::vec3(-1,  1,  1), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),
            Model::VertexData(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),
            Model::VertexData(glm::vec3(1,  1, -1), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)),
            Model::VertexData(glm::vec3(-1,  1, -1), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1))
        };

        std::vector<uint32_t> indices = {
            // Front Face
            0, 1, 2,  0, 2, 3,
            // Back Face
            4, 5, 6,  4, 6, 7,
            // Left Face
            8, 9, 10,  8, 10, 11,
            // Right Face
            12, 13, 14,  12, 14, 15,
            // Bottom Face
            16, 17, 18,  16, 18, 19,
            // Top Face
            20, 21, 22,  20, 22, 23
        };

        return Model::createStaticMesh(vertices, indices);
	}

}