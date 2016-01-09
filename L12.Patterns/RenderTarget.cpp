
#include "Utility.h"
#include "RenderTarget.h"

using namespace Learnings;

#define RESTORE_DEFERRED_CONTEXT_STATE FALSE // recommended by MSDN

RenderTarget::RenderTarget(GraphicsDevice::Context context, GraphicsDevice::RenderTargetView rtv, GraphicsDevice::DepthStencilView dsv, D3D11_VIEWPORT viewport)
	: m_Context(context),
	m_RTV(rtv),
	m_DSV(dsv),
	m_Viewport(viewport),
	m_CommandList(nullptr),
	m_ContextType(Type::Immediate)
{
	if (m_Context->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED)
		m_ContextType = Type::Deferred;
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::ReleaseViewBuffers()
{
	m_RTV.Release();
	m_DSV.Release();
	m_Viewport = {};
}

void RenderTarget::UpdateViewBuffers(GraphicsDevice::RenderTargetView rtv, GraphicsDevice::DepthStencilView dsv, D3D11_VIEWPORT viewport)
{
	m_RTV = rtv;
	m_DSV = dsv;
	m_Viewport = viewport;
}

void RenderTarget::Clear(const std::array<float, 4u>& color)
{
	m_Context->ClearRenderTargetView(m_RTV, color.data());
	m_Context->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderTarget::SetInputType(GraphicsDevice::InputLayout il, D3D11_PRIMITIVE_TOPOLOGY tp)
{
	if (il)
	{
		m_Context->IASetInputLayout(il);
	}

	if (tp)
	{
		m_Context->IASetPrimitiveTopology(tp);
	}
}

void RenderTarget::SetShader(GraphicsDevice::VertexShader vs, GraphicsDevice::PixelShader ps)
{
	if (vs)
	{
		m_Context->VSSetShader(vs, NULL, NULL);
	}
	if (ps)
	{
		m_Context->PSSetShader(ps, NULL, NULL);
	}
}

void RenderTarget::SetStates(GraphicsDevice::BlendState bs, GraphicsDevice::DepthStencilState ds, GraphicsDevice::RasterizerState rs, GraphicsDevice::SamplerState ss)
{
	if (bs)
	{
		//std::array<float, 4> color = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_Context->OMSetBlendState(bs, NULL /*color*/, 0xFFFFFFFF);
	}

	if (ds)
	{
		m_Context->OMSetDepthStencilState(ds, NULL);
	}

	if (rs)
	{
		m_Context->RSSetState(rs);
	}

	if (ss)
	{
		m_Context->PSSetSamplers(0, 1, &(ss.p));
	}

	m_Context->OMSetRenderTargets(1, &(m_RTV.p), m_DSV);
	m_Context->RSSetViewports(1, &m_Viewport);
}

void RenderTarget::SetConstantBuffers(const std::vector< std::tuple<Stage, GraphicsDevice::Buffer, uint16_t> > &buffers)
{
	for (auto &buffer : buffers)
	{
		Stage stage;
		GraphicsDevice::Buffer cb;
		uint16_t index;

		std::tie(stage, cb, index) = buffer;

		switch (stage)
		{
			case Stage::Vertex:
				m_Context->VSSetConstantBuffers(index, 1, &(cb.p));
				break;
			case Stage::Pixel:
				m_Context->PSSetConstantBuffers(index, 1, &(cb.p));
				break;
		}
	}
}

void RenderTarget::SetShaderResource(const std::vector< std::tuple<Stage, GraphicsDevice::ShaderResourceView, uint16_t> > &resources)
{
	for (auto &resource : resources)
	{
		Stage stage;
		GraphicsDevice::ShaderResourceView srv;
		uint16_t index;

		std::tie(stage, srv, index) = resource;

		switch (stage)
		{
			case Stage::Vertex:
				m_Context->VSSetShaderResources(index, 1, &(srv.p));
				break;
			case Stage::Pixel:
				m_Context->PSSetShaderResources(index, 1, &(srv.p));
				break;
		}
	}

}

void RenderTarget::SetMeshData(GraphicsDevice::Buffer vb, uint32_t vertexSize, GraphicsDevice::Buffer ib)
{
	UINT vertexOffset = 0, indexOffset = 0;

	m_Context->IASetVertexBuffers(0, 1, &(vb.p), &vertexSize, &vertexOffset);
	m_Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, indexOffset);
}

void RenderTarget::Draw(uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart)
{
	m_Context->DrawIndexed(indexCount, indexStart, vertexStart);
}

void RenderTarget::Finish()
{
	if (m_ContextType == Type::Deferred)
	{
		m_CommandList.Release();

		m_Context->FinishCommandList(RESTORE_DEFERRED_CONTEXT_STATE, &(m_CommandList.p));
	}
}

GraphicsDevice::CommandList RenderTarget::CommandList() const
{
	return m_CommandList;
}

RenderTarget::Type RenderTarget::GetType() const
{
	return m_ContextType;
}
