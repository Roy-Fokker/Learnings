#include "Vertex.h"

using namespace Learnings;

// VertexPositionTexture
const std::array<D3D11_INPUT_ELEMENT_DESC, VertexPositionTexture::C_VertexElementCount> VertexPositionTexture::ElementsDesc{ {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
} };

const uint32_t VertexPositionTexture::Size = sizeof(VertexPositionTexture);



uint32_t Mesh::VertexListSize() const
{
	return static_cast<uint32_t>(vertices.size()) * VertexPositionTexture::Size;
}

uint32_t Mesh::IndexListSize() const
{
	return static_cast<uint32_t>(indices.size()) * sizeof(uint32_t);
}

uint32_t Mesh::VertexStride()
{
	return VertexPositionTexture::Size;
}