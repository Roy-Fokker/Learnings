#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#ifdef _DEBUG
#pragma comment(lib, "dxguid.lib")
#endif

#define ThrowIfFailed(hr, msg) \
if ( FAILED(hr) ) \
{ \
  throw std::runtime_error(msg); \
}

#include <array>
#include <vector>
#include <d3dcommon.h>

#include <DDSTextureLoader.cpp>

#include "Direct3D.h"
#include "Mesh.h"

using namespace Learnings;

namespace
{
	static const DXGI_FORMAT C_SwapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const UINT C_MSAA_SampleCount = 16U;
	
	inline void GetWindowSize(HWND hWnd, uint16_t &width, uint16_t &height)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);

		height = (uint16_t)(rect.bottom - rect.top);
		width = (uint16_t)(rect.right - rect.left);
	}

	inline DXGI_SAMPLE_DESC QueryMsaaLevel(ID3D11Device *device)
	{
		DXGI_SAMPLE_DESC sd{ 1, 0 };

#ifdef DEBUG
		return sd;
#else
		UINT msaa = 0;
		UINT sampleCount = C_MSAA_SampleCount;
		HRESULT hr = device->CheckMultisampleQualityLevels(C_SwapChainFormat, sampleCount, &msaa);
		if (SUCCEEDED(hr))
		{
			if (msaa > 0)
			{
				sd.Count = sampleCount;
				sd.Quality = msaa - 1;
			}
		}

		return sd;
#endif
	}

	inline DXGI_RATIONAL QueryRefreshRate(CComPtr<IDXGIAdapter> dxgiAdapter, uint16_t width, uint16_t height, BOOL vSync)
	{
		DXGI_RATIONAL refreshRate{ 0, 1 };

		if (vSync)
		{
			HRESULT hr;

			CComPtr<IDXGIOutput> adapterOutput;
			hr = dxgiAdapter->EnumOutputs(0, &adapterOutput);
			ThrowIfFailed(hr, "Failed to enumerate adapter outputs");

			uint32_t displayModesCnt = 0;
			hr = adapterOutput->GetDisplayModeList(C_SwapChainFormat,
												   DXGI_ENUM_MODES_INTERLACED,
												   &displayModesCnt,
												   nullptr);
			ThrowIfFailed(hr, "Failed to get display modes count");


			std::vector<DXGI_MODE_DESC> displayModes(displayModesCnt);
			hr = adapterOutput->GetDisplayModeList(C_SwapChainFormat,
												   DXGI_ENUM_MODES_INTERLACED,
												   &displayModesCnt,
												   &displayModes[0]);
			ThrowIfFailed(hr, "Failed to get display modes list");

			for (auto &mode : displayModes)
			{
				if (mode.Width == width && mode.Height == height)
				{
					refreshRate = mode.RefreshRate;
				}
			}
		}

		return refreshRate;
	}
}

Direct3d::Direct3d(HWND hWnd) :
	m_hWnd(hWnd)
{
	CreateDevice();
	CreateSwapChain();
	CreateViewBuffer();
}

Direct3d::~Direct3d()
{
	DeleteViewBuffer();
	DeleteSwapChain();
	DeleteDevice();
}

void Direct3d::Clear()
{
	static std::array<float, 4> color{ 0.25f, 0.25f, 0.5f, 1.0f };

	m_Context->ClearRenderTargetView(m_RenderTargetView, &color[0]);
	m_Context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Direct3d::Present()
{
	m_SwapChain->Present(m_vSync, 0);
}

void Direct3d::Resize()
{
	DeleteViewBuffer();
	CreateViewBuffer();
}

Direct3d::Context Direct3d::GetContext() const
{
	return m_Context;
}

Direct3d::Device Direct3d::GetDevice() const
{
	return m_Device;
}

Direct3d::Buffer Direct3d::CreateBuffer(uint32_t size, const void *data, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, UINT accessFlags)
{
	D3D11_BUFFER_DESC bd{ 0 };
	bd.Usage = usage;
	bd.ByteWidth = size;
	bd.BindFlags = bindFlags;
	bd.CPUAccessFlags = accessFlags;

	D3D11_SUBRESOURCE_DATA bData{ 0 };
	bData.pSysMem = data;

	Buffer buf;
	HRESULT hr = m_Device->CreateBuffer(&bd, &bData, &(buf.p));
	ThrowIfFailed(hr, "Failed to create buffer");

	return buf;
}

Direct3d::VertexShader Direct3d::CreateVertexShader(uint32_t size, const void *vs)
{
	VertexShader vsh;

	HRESULT hr = m_Device->CreateVertexShader(vs, size, NULL, &vsh);
	ThrowIfFailed(hr, "Failed to create vertex shader");

	return vsh;
}

Direct3d::PixelShader Direct3d::CreatePixelShader(uint32_t size, const void *ps)
{
	PixelShader psh;
	HRESULT hr = m_Device->CreatePixelShader(ps, size, NULL, &psh);
	ThrowIfFailed(hr, "Failed to create pixel shader");

	return psh;
}

Direct3d::InputLayout Direct3d::CreateInputLayout(uint32_t elemCount, const D3D11_INPUT_ELEMENT_DESC * elemDesc, uint32_t size, const void *vs)
{
	InputLayout il;

	HRESULT hr = m_Device->CreateInputLayout(elemDesc,
										   elemCount,
										   vs, size,
										   &il);
	ThrowIfFailed(hr, "Failed to create input layout");

	return il;
}

bool Direct3d::CheckInputLayout(uint32_t elemCount, const D3D11_INPUT_ELEMENT_DESC * elemDesc, uint32_t size, const void *vs)
{
	HRESULT hr = m_Device->CreateInputLayout(elemDesc,
										   elemCount,
										   vs, size,
										   NULL);
	// CreateInputLayout returns S_FALSE, 
	// if the signature of the InputLayout Matches that of the Shader's Inputs
	if (hr == S_FALSE)
	{
		return true;
	}

	return false;
}

Direct3d::ShaderResourceView Direct3d::CreateShaderResourceView(uint32_t size, const uint8_t *tex)
{
	HRESULT hr;

	ShaderResourceView view;

	hr = DirectX::CreateDDSTextureFromMemory(m_Device,
											 tex,
											 size,
											 nullptr,
											 &view);
	ThrowIfFailed(hr, "Cannot create shader resource view");

	return view;
}

Direct3d::ShaderResourceView Direct3d::CreateShaderResourceView(Texture2d texture)
{
	HRESULT hr;

	ShaderResourceView view;
	hr = m_Device->CreateShaderResourceView(texture, nullptr, &view);
	ThrowIfFailed(hr, "Cannot create shader resource view");

	return view;
}

Direct3d::BlendState Direct3d::CreateBlendState(D3D11_BLEND src, D3D11_BLEND srcAlpha, D3D11_BLEND dst, D3D11_BLEND dstAlpha, D3D11_BLEND_OP op, D3D11_BLEND_OP opAlpha)
{
	D3D11_BLEND_DESC bd{ 0 };

	bd.RenderTarget[0].BlendEnable = ((src != D3D11_BLEND_ONE) || (dst != D3D11_BLEND_ONE));

	bd.RenderTarget[0].SrcBlend = src;
	bd.RenderTarget[0].BlendOp = op;
	bd.RenderTarget[0].DestBlend = dst;

	bd.RenderTarget[0].SrcBlendAlpha = srcAlpha;
	bd.RenderTarget[0].BlendOpAlpha = opAlpha;
	bd.RenderTarget[0].DestBlendAlpha = dstAlpha;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendState state = nullptr;
	HRESULT hr = m_Device->CreateBlendState(&bd, &state);
	ThrowIfFailed(hr, "Failed to create blend state");

	return state;
}

Direct3d::DepthStencilState Direct3d::CreateDepthStencilState(bool depthEnable, bool writeEnable)
{
	D3D11_DEPTH_STENCIL_DESC dsd{ 0 };

	dsd.DepthEnable = depthEnable;
	dsd.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dsd.StencilEnable = false;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	
	dsd.BackFace = dsd.FrontFace;

	DepthStencilState state;
	HRESULT hr = m_Device->CreateDepthStencilState(&dsd, &state);
	ThrowIfFailed(hr, "Failed to create depth stencil state");

	return state;
}

Direct3d::RasterizerState Direct3d::CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode)
{
	D3D11_RASTERIZER_DESC rd{};

	rd.CullMode = cullMode;
	rd.FillMode = fillMode;
	rd.DepthClipEnable = true;
	rd.MultisampleEnable = true;
	
	RasterizerState state = nullptr;
	HRESULT hr = m_Device->CreateRasterizerState(&rd, &state);
	ThrowIfFailed(hr, "Failed to create rasterizer state");
	
	return state;
}

Direct3d::SamplerState Direct3d::CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, uint32_t maxAnisotropy)
{
	D3D11_SAMPLER_DESC sd{ };

	sd.Filter = filter;

	sd.AddressU = textureAddressMode;
	sd.AddressV = textureAddressMode;
	sd.AddressW = textureAddressMode;

	sd.MaxAnisotropy = maxAnisotropy;

	sd.MaxLOD = FLT_MAX;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;

	SamplerState state = nullptr;
	HRESULT hr = m_Device->CreateSamplerState(&sd, &state);
	ThrowIfFailed(hr, "Failed to create sampler state");

	return state;
}

Direct3d::Texture2d Direct3d::CreateTexture2d(uint32_t width, uint32_t height, uint32_t bindFlags, D3D11_USAGE usage, DXGI_FORMAT format, DXGI_SAMPLE_DESC sampleDesc, uint16_t arraysize, uint16_t miplevels)
{
	D3D11_TEXTURE2D_DESC td{ };

	td.Width = width;
	td.Height = height;

	td.BindFlags = bindFlags;
	td.Usage = usage;
	td.Format = format;
	td.SampleDesc = sampleDesc;

	td.ArraySize = arraysize;
	td.MipLevels = miplevels;

	Texture2d texture;
	HRESULT hr = m_Device->CreateTexture2D(&td, nullptr, &texture);
	ThrowIfFailed(hr, "Failed to create texture");

	return texture;
}

void Direct3d::CreateDevice()
{
	uint32_t flags = NULL;
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::array<D3D_FEATURE_LEVEL, 1> featureLevels{
		D3D_FEATURE_LEVEL_11_0
	};

	HRESULT hr = D3D11CreateDevice(nullptr,
								   D3D_DRIVER_TYPE_HARDWARE,
								   nullptr,
								   flags,
								   &featureLevels[0],
								   static_cast<uint32_t>(featureLevels.size()),
								   D3D11_SDK_VERSION,
								   &m_Device,
								   nullptr,
								   &m_Context);
	ThrowIfFailed(hr, "Failed to create direct3d device and context");
}

void Direct3d::DeleteDevice()
{
	m_Context.Release();
	m_Device.Release();
}

void Direct3d::CreateSwapChain()
{
	HRESULT hr;

	CComPtr<IDXGIDevice> dxgiDevice;
	hr = m_Device->QueryInterface<IDXGIDevice>(&dxgiDevice);
	ThrowIfFailed(hr, "Failed to get DXGI Device");

	CComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgiAdapter);
	ThrowIfFailed(hr, "Failed to get DXGI Adapter");

	CComPtr<IDXGIFactory> dxgiFactory;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
	ThrowIfFailed(hr, "Failed to get DXGI Factory");

	uint16_t width, height;
	GetWindowSize(m_hWnd, width, height);

	DXGI_SWAP_CHAIN_DESC sd = { 0 };

	sd.BufferCount = 1;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Format = C_SwapChainFormat;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.RefreshRate = QueryRefreshRate(dxgiAdapter, width, height, m_vSync);
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc = QueryMsaaLevel(m_Device.p);
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.Windowed = TRUE;

	hr = dxgiFactory->CreateSwapChain(m_Device, &sd, &m_SwapChain);
	ThrowIfFailed(hr, "Failed to create SwapChain");

	dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
}

void Direct3d::DeleteSwapChain()
{
	m_SwapChain.Release();
}

void Direct3d::CreateViewBuffer()
{
	HRESULT hr;

	hr = m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	ThrowIfFailed(hr, "Failed to resize swap chain buffer");

	DXGI_SWAP_CHAIN_DESC sd{ 0 };
	hr = m_SwapChain->GetDesc(&sd);
	ThrowIfFailed(hr, "Failed to get swap chain description");

	uint16_t width = static_cast<uint16_t>(sd.BufferDesc.Width);
	uint16_t height = static_cast<uint16_t>(sd.BufferDesc.Height);

	CreateRenderTargetView();
	CreateDepthStencilBuffer(width, height);
	CreateDepthStencilView();

	D3D11_VIEWPORT viewport{ 0 };
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MaxDepth = 1.0f;

	m_Context->OMSetRenderTargets(1, &(m_RenderTargetView.p), m_DepthStencilView);
	m_Context->RSSetViewports(1, &viewport);
}

void Direct3d::DeleteViewBuffer()
{
	DeleteDepthStencilView();
	DeleteDepthStencilBuffer();
	DeleteRenderTargetView();
}

void Direct3d::CreateRenderTargetView()
{
	HRESULT hr;

	Texture2d buffer = nullptr;
	hr = m_SwapChain->GetBuffer(0, 
								__uuidof(ID3D11Texture2D), 
								reinterpret_cast<void **>(&buffer.p));
	ThrowIfFailed(hr, "Failed to get swap chain buffer");
	
	hr = m_Device->CreateRenderTargetView(buffer, 0, &m_RenderTargetView);
	ThrowIfFailed(hr, "Failed to create render target view");
}

void Direct3d::DeleteRenderTargetView()
{
	m_RenderTargetView.Release();
}

void Direct3d::CreateDepthStencilBuffer(uint16_t width, uint16_t height)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC td{ 0 };
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc = QueryMsaaLevel(m_Device.p);
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = m_Device->CreateTexture2D(&td, 0, &m_DepthStencilBuffer);
	ThrowIfFailed(hr, "Failed to create depth stencil buffer");
}

void Direct3d::DeleteDepthStencilBuffer()
{
	m_DepthStencilBuffer.Release();
}

void Direct3d::CreateDepthStencilView()
{
	HRESULT hr;

	hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView);
	ThrowIfFailed(hr, "Failed to create depth stencil view");
}

void Direct3d::DeleteDepthStencilView()
{
	m_DepthStencilView.Release();
}
