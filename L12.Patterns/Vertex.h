#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <DirectXMath.h>
#include <d3d11.h>

namespace Learnings
{
	struct VertexPositionTexture
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;

		static const uint8_t C_VertexElementCount = 2;
		static const std::array<D3D11_INPUT_ELEMENT_DESC, C_VertexElementCount> ElementsDesc;
		static const uint32_t Size;
		static const uint32_t Id;
	};

	struct Mesh
	{
		typedef std::vector<VertexPositionTexture> VertexList;
		typedef std::vector<uint32_t> IndexList;

		VertexList vertices;
		IndexList indices;

		uint32_t VertexListSize() const;
		uint32_t IndexListSize() const;

		static uint32_t VertexStride();
	};

}