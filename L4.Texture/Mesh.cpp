#include "Mesh.h"

using namespace Learnings;

const std::array<D3D11_INPUT_ELEMENT_DESC, Vertex::C_VertexElementCount> Learnings::Vertex::ElementsDesc{ {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
} };

const uint32_t Vertex::Size = sizeof(Learnings::Vertex);