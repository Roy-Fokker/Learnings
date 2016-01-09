#pragma once

#include "Services.h"
#include "Direct3D.h"

namespace Learnings
{
	class RenderTarget : public Service
	{
	public:
		enum class Type
		{
			Immediate,
			Deferred
		};

		enum class Stage
		{
			Vertex,
			Pixel
		};

	public:
		RenderTarget(GraphicsDevice::Context context, GraphicsDevice::RenderTargetView rtv, GraphicsDevice::DepthStencilView dsv, D3D11_VIEWPORT viewport);
		~RenderTarget();

		void ReleaseViewBuffers();
		void UpdateViewBuffers(GraphicsDevice::RenderTargetView rtv, GraphicsDevice::DepthStencilView dsv, D3D11_VIEWPORT viewport);

		void Finish();
		GraphicsDevice::CommandList CommandList() const;
		Type GetType() const;

		void Clear(const std::array<float, 4u> &color);

		void SetInputType(GraphicsDevice::InputLayout il, D3D11_PRIMITIVE_TOPOLOGY tp);
		void SetShader(GraphicsDevice::VertexShader vs, GraphicsDevice::PixelShader ps);
		void SetStates(GraphicsDevice::BlendState bs, GraphicsDevice::DepthStencilState ds, GraphicsDevice::RasterizerState rs, GraphicsDevice::SamplerState ss);
		void SetConstantBuffers(const std::vector< std::tuple<Stage, GraphicsDevice::Buffer, uint16_t> > &buffers);
		void SetShaderResource(const std::vector< std::tuple<Stage, GraphicsDevice::ShaderResourceView, uint16_t> > &resources);
		void SetMeshData(GraphicsDevice::Buffer vb, uint32_t vertexSize, GraphicsDevice::Buffer ib);

		void Draw(uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart);

	private:
		GraphicsDevice::Context m_Context;
		GraphicsDevice::RenderTargetView m_RTV;
		GraphicsDevice::DepthStencilView m_DSV;
		D3D11_VIEWPORT m_Viewport;

		GraphicsDevice::CommandList m_CommandList;
		Type m_ContextType;
	};
}