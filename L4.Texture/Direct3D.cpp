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

#include "Direct3D.h"
#include "Mesh.h"

using namespace Learnings;

namespace
{
	static const DXGI_FORMAT C_SwapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	
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

		UINT msaa = 0;
		UINT sampleCount = 4U;
		HRESULT hr = device->CheckMultisampleQualityLevels(C_SwapChainFormat, sampleCount, &msaa);
		if (SUCCEEDED(hr))
		{
			if (msaa - 1 > 0)
			{
				sd.Count = sampleCount;
				sd.Quality = msaa - 1;
			}
		}

		return sd;
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

Direct3d::ShaderResourceView Direct3d::CreateShaderResourceView(uint32_t size, const void *tex)
{
	
}

void Direct3d::CreateDevice()
{
	uint32_t flags = NULL;
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

	CComPtr<ID3D11Texture2D> buffer = nullptr;
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
