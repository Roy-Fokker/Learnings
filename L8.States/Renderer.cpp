#include <algorithm>

#include <DirectXColors.h>

#include "Renderer.h"
#include "Mesh.h"

using namespace Learnings;

Renderer::Renderer(HWND hWnd)
{
	m_d3d = std::make_unique<Learnings::Direct3d>(hWnd);

	CreateStates();
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

		context->OMSetBlendState(m_BlendState,
								 DirectX::Colors::Transparent,
								 0xFFFFFFFF);
		context->OMSetDepthStencilState(m_DepthStencilState,
										NULL);
		context->RSSetState(m_RasterizerState);
		context->PSSetSamplers(0, 1, &(m_SampleState.p));

		context->VSSetShader(m_VertexShader, NULL, NULL);
		context->PSSetShader(m_PixelShader, NULL, NULL);

		context->PSSetShaderResources(0, 1, &(m_ShaderResourceView.p));

		context->IASetInputLayout(m_InputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->VSSetConstantBuffers(slot,
									  count,
									  &(m_ProjectionBuffer.p));

		for (auto &mesh : m_Meshes)
		{
			context->IASetVertexBuffers(0,
										1,
										&(mesh.vertexBuffer.p),
										&Vertex::Size,
										&vertexOffset);
			context->IASetIndexBuffer(mesh.indexBuffer,
									  DXGI_FORMAT_R32_UINT,
									  indexOffset);

			auto itRange = m_TransformBuffer.equal_range(mesh.id);
			std::for_each(itRange.first, itRange.second, [&](auto& it)
			{
				auto transformBuffer = it.second;
				context->VSSetConstantBuffers(slot + 1,
											  count,
											  &(transformBuffer.p));

				context->DrawIndexed(mesh.indexCount, 0, 0);
			});
		}
	}
	m_d3d->Present();
}

void Renderer::Resize()
{
	m_d3d->Resize();
}

void Renderer::AddGeometry(uint32_t meshId, const Mesh &mesh)
{
	uint32_t mId = (uint32_t)m_Meshes.size();

	auto it = m_MeshIds.find(meshId);
	if (it != m_MeshIds.end())
	{
		mId = it->second;
	}
	else
	{
		m_Meshes.push_back(RenderableMesh());
	}

	auto &mo = m_Meshes[mId];

	mo.id = mId;
	
	mo.vertexBuffer = m_d3d->CreateBuffer((uint32_t)mesh.vertices.size() * Vertex::Size,
										 mesh.vertices.data(),
										 D3D11_BIND_VERTEX_BUFFER,
										 D3D11_USAGE_DEFAULT,
										 NULL);

	mo.indexBuffer = m_d3d->CreateBuffer((uint32_t)mesh.indices.size() * sizeof(uint32_t),
										mesh.indices.data(),
										D3D11_BIND_INDEX_BUFFER,
										D3D11_USAGE_DEFAULT,
										NULL);
	
	mo.indexCount = (uint32_t)mesh.indices.size();
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

void Renderer::SetTransforms(uint32_t meshId, uint32_t instanceId, const Transform &transform)
{
	auto instanceCount = m_TransformBuffer.count(meshId);

	if (instanceCount + 1 < instanceId && instanceId != 0)
	{
		return;
	}

	auto itRange = m_TransformBuffer.equal_range(meshId);
	auto it = std::next(itRange.first, instanceId);
	if (itRange.first == m_TransformBuffer.end() ||
		it == m_TransformBuffer.end())
	{

		auto transformBuffer = m_d3d->CreateBuffer(sizeof(Transform),
												   &transform,
												   D3D11_BIND_CONSTANT_BUFFER,
												   D3D11_USAGE_DYNAMIC,
												   D3D11_CPU_ACCESS_WRITE);

		m_TransformBuffer.insert({ meshId, transformBuffer });
	}
	else
	{
		auto transformBuffer = it->second;

		auto context = m_d3d->GetContext();
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE buffer;

		hr = context->Map(transformBuffer,
						  NULL,
						  D3D11_MAP_WRITE_DISCARD,
						  NULL,
						  &buffer);
		assert(hr == S_OK && "constant buffer could not be locked"); // Do something here?????
		
		Transform *to = reinterpret_cast<Transform *>(buffer.pData);
		to->matrix = transform.matrix;

		context->Unmap(transformBuffer,
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

void Renderer::CreateStates()
{
	m_BlendState = m_d3d->CreateBlendState(D3D11_BLEND_ONE, 
										   D3D11_BLEND_ONE, 
										   D3D11_BLEND_ZERO, 
										   D3D11_BLEND_ZERO, 
										   D3D11_BLEND_OP_ADD, 
										   D3D11_BLEND_OP_ADD);

	m_DepthStencilState = m_d3d->CreateDepthStencilState(true,
														 true);

	m_RasterizerState = m_d3d->CreateRasterizerState(D3D11_CULL_BACK,
													 D3D11_FILL_SOLID);

	m_SampleState = m_d3d->CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT,
											  D3D11_TEXTURE_ADDRESS_WRAP,
											  16);
}

void Renderer::DeleteStates()
{
	m_BlendState.Release();
	m_DepthStencilState.Release();
	m_RasterizerState.Release();
	m_SampleState.Release();
}
