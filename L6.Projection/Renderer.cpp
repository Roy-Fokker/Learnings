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
		uint32_t slot = 0, count = 1;
		auto context = m_d3d->GetContext();

		context->VSSetShader(m_VertexShader, NULL, NULL);
		context->PSSetShader(m_PixelShader, NULL, NULL);

		context->PSSetShaderResources(0, 1, &(m_ShaderResourceView.p));

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
		
		context->VSSetConstantBuffers(slot,
									  count,
									  &(m_ProjectionBuffer.p));
		context->VSSetConstantBuffers(slot + 1,
									  count,
									  &(m_TransformBuffer.p));

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

void Renderer::AddShader(const std::vector<byte> &vs, const std::vector<byte> &ps)
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

void Renderer::AddTexture(const std::vector<byte> &tex)
{
	m_ShaderResourceView = m_d3d->CreateShaderResourceView((uint32_t)tex.size(),
														   tex.data());
}

void Renderer::SetTransforms(const Transform &transform)
{
	if (!m_TransformBuffer)
	{
		m_TransformBuffer = m_d3d->CreateBuffer(sizeof(Transform),
												&transform,
												D3D11_BIND_CONSTANT_BUFFER,
												D3D11_USAGE_DYNAMIC,
												D3D11_CPU_ACCESS_WRITE);
	}
	else
	{
		auto context = m_d3d->GetContext();
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE buffer;

		hr = context->Map(m_TransformBuffer, 
						  NULL,
						  D3D11_MAP_WRITE_DISCARD,
						  NULL,
						  &buffer);
		assert(hr == S_OK && "constant buffer could not be locked"); // Do something here?????
		
		Transform *to = reinterpret_cast<Transform *>(buffer.pData);
		to->matrix = transform.matrix;

		context->Unmap(m_TransformBuffer, 
					   NULL);
	}
}

void Renderer::SetProjection(const Projection &projection)
{
	if (!m_ProjectionBuffer)
	{
		m_ProjectionBuffer = m_d3d->CreateBuffer(sizeof(Transform),
												&projection,
												D3D11_BIND_CONSTANT_BUFFER,
												D3D11_USAGE_DYNAMIC,
												D3D11_CPU_ACCESS_WRITE);
	}
	else
	{
		auto context = m_d3d->GetContext();
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE buffer;

		hr = context->Map(m_ProjectionBuffer,
						  NULL,
						  D3D11_MAP_WRITE_DISCARD,
						  NULL,
						  &buffer);
		assert(hr == S_OK && "constant buffer could not be locked"); // Do something here?????

		Transform *to = reinterpret_cast<Transform *>(buffer.pData);
		to->matrix = projection.matrix;

		context->Unmap(m_ProjectionBuffer,
					   NULL);
	}
}
