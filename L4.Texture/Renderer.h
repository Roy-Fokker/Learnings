#pragma once

#include <memory>
#include <vector>
#include "Direct3D.h"

namespace Learnings
{
	struct Mesh;

	class Renderer
	{
	public:
		Renderer(HWND hWnd);
		~Renderer();

		void Draw();
		void Resize();

		void AddGeometry(const Mesh &mesh);
		void AddShader(const std::vector<byte> &vs, const std::vector<byte> &ps);
		void AddTexture(const std::vector<byte> &tex);

	private:
		std::unique_ptr<Learnings::Direct3d> m_d3d;


		Direct3d::Buffer m_VertexBuffer;
		Direct3d::Buffer m_IndexBuffer;
		uint32_t m_IndexCount;

		Direct3d::VertexShader m_VertexShader;
		Direct3d::PixelShader m_PixelShader;
		Direct3d::InputLayout m_InputLayout;
	};
}