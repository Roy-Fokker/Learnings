#include <algorithm>

#include <DirectXColors.h>

#include "Renderer.h"
#include "Mesh.h"

using namespace Learnings;

/* Direct2D test */
#include <d2d1_1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define ThrowIfFailed(hr, msg) \
if ( FAILED(hr) ) \
{ \
	throw std::runtime_error(msg); \
}

void Renderer::AddText(const std::wstring & text)
{
	HRESULT hr;

	// Texture to write to
	auto texture = m_d3d->CreateTexture2d(512,
										  512,
										  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
										  D3D11_USAGE_DEFAULT,
										  DXGI_FORMAT_R8G8B8A8_UNORM,
										  { 1, 0 },
										  1, 1);
	// Convert to ShaderResource for Direct3d use
	m_ShaderResourceView = m_d3d->CreateShaderResourceView(texture);

	// Convert to DXGISurface for Direct2d to consume
	CComPtr<IDXGISurface1> dxgiSurface;
	texture->QueryInterface<IDXGISurface1>(&dxgiSurface);


	// Get DXGI device from D3D device
	CComPtr<IDXGIDevice> dxgiDevice;
	hr = m_d3d->GetDevice()->QueryInterface<IDXGIDevice>(&dxgiDevice);
	ThrowIfFailed(hr, "Failed to get DXGI Device from D3D11Device");

	// Get Direct 2D Device from DXGI
	CComPtr<ID2D1Device> d2dDevice;
	hr = D2D1CreateDevice(dxgiDevice, nullptr, &d2dDevice);
	ThrowIfFailed(hr, "Failed to create Direct2D device");

	// Create Direct 2D Device Context
	D2D1_DEVICE_CONTEXT_OPTIONS opts = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	CComPtr<ID2D1DeviceContext> d2dContext;
	hr = d2dDevice->CreateDeviceContext(opts, &d2dContext);
	ThrowIfFailed(hr, "Failed to create Direct2D device context");

	// Convert the texture to Direct2d surface
	D2D1_BITMAP_PROPERTIES1 props{};
	props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
	CComPtr<ID2D1Bitmap1> d2dBitmap;
	d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface, props, &d2dBitmap);

	// Set Direct2d Bitmap as target
	d2dContext->SetTarget(d2dBitmap);

	// Create brush to write with
	CComPtr<ID2D1SolidColorBrush> brush;
	hr = d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), &brush);
	ThrowIfFailed(hr, "Failed to create a solid brush");

	// Direct Write
	CComPtr<IDWriteFactory> writeFactory;
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
							 __uuidof(writeFactory),
							 reinterpret_cast<IUnknown * *>(&writeFactory));
	ThrowIfFailed(hr, "Failed to create DirectWrite Factory");

	// Create DirectWrite text format
	CComPtr<IDWriteTextFormat> textFormat;
	hr = writeFactory->CreateTextFormat(L"Consolas", 
										nullptr, 
										DWRITE_FONT_WEIGHT_NORMAL, 
										DWRITE_FONT_STYLE_NORMAL, 
										DWRITE_FONT_STRETCH_NORMAL, 
										100.0f,
										L"", 
										&textFormat);
	ThrowIfFailed(hr, "Failed to create DirectWrite text format");

	// Create DirectWrite text layout
	CComPtr<IDWriteTextLayout> textLayout;
	hr = writeFactory->CreateTextLayout(text.c_str(),
										(uint32_t)text.length(),
										textFormat,
										512, 512,
										&textLayout);
	ThrowIfFailed(hr, "Failed to create DirectWrite text layout");

	d2dContext->BeginDraw();
	d2dContext->Clear(D2D1::ColorF(0, 0.0f));
		
	d2dContext->DrawTextLayout({ 0, 0 },
								 textLayout,
								 brush);

	d2dContext->EndDraw();

}

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
		
		context->VSSetConstantBuffers(slot,
									  count,
									  &(m_ProjectionBuffer.p));

		for (auto &mesh : m_Meshes)
		{
			auto it = m_TopologyRules.find(mesh.id);
			if (it != m_TopologyRules.end())
			{
				context->IASetPrimitiveTopology(it->second);
			}
			else
			{
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}

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
		m_MeshIds[mId] = meshId;
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

void Learnings::Renderer::SetTopology(uint32_t meshId, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	auto it = m_MeshIds.find(meshId);

	if (it == m_MeshIds.end())
	{
		return;
	}

	m_TopologyRules[meshId] = topology;
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
										   D3D11_BLEND_SRC_ALPHA,
										   D3D11_BLEND_SRC_ALPHA,
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
