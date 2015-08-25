#include "Renderer.h"

#include "Mesh.h"

using namespace Learnings;

Renderer::Renderer(HWND hWnd)
{
	m_d3d = std::make_unique<Learnings::Direct3d>(hWnd);
}

Renderer::~Renderer()
{
}

void Renderer::Draw()
{
	m_d3d->Clear();
	{
		uint32_t vertexOffset = 0, indexOffset = 0;
		auto context = m_d3d->GetContext();

		context->VSSetShader(m_VertexShader, NULL, NULL);
		context->PSSetShader(m_PixelShader, NULL, NULL);

		context->IASetInputLayout(m_InputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetVertexBuffers(0,
									1,
									&(m_VertexBuffer.p),
									&Vertex::Size,
									&vertexOffset);
		context->IASetIndexBuffer(m_IndexBuffer,
								  DXGI_FORMAT_R32_UINT,
								  indexOffset);

		context->DrawIndexed(m_IndexCount, 0, 0);
	}
	m_d3d->Present();
}

void Renderer::Resize()
{
	m_d3d->Resize();
}

void Renderer::AddGeometry(const Mesh &mesh)
{
	m_VertexBuffer = m_d3d->CreateBuffer((uint32_t)mesh.vertices.size() * Vertex::Size,
										 mesh.vertices.data(),
										 D3D11_BIND_VERTEX_BUFFER,
										 D3D11_USAGE_DEFAULT,
										 NULL);

	m_IndexBuffer = m_d3d->CreateBuffer((uint32_t)mesh.indices.size() * sizeof(uint32_t),
										mesh.indices.data(),
										D3D11_BIND_INDEX_BUFFER,
										D3D11_USAGE_DEFAULT,
										NULL);
	m_IndexCount = (uint32_t)mesh.indices.size();
}

void Renderer::AddShader(const std::vector<byte>& vs, const std::vector<byte>& ps)
{
	m_InputLayout = m_d3d->CreateInputLayout(Vertex::C_VertexElementCount,
											 Vertex::ElementsDesc.data(),
											 (uint32_t)vs.size(),
											 vs.data());

	m_VertexShader = m_d3d->CreateVertexShader((uint32_t)vs.size(),
											   vs.data());

	m_PixelShader = m_d3d->CreatePixelShader((uint32_t)ps.size(),
											 ps.data());
}

void Learnings::Renderer::AddTexture(const std::vector<byte>& tex)
{
}
