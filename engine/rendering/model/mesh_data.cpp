#include "mesh_data.h"

MeshData::MeshData(std::vector<VertexData> _vertices, std::vector<unsigned int> _indices, std::vector<Texture*> _textures)
{
	vertices = _vertices;
	indices = _indices;
	textures = _textures;
}
