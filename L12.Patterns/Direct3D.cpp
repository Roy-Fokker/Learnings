#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "dxguid.lib")
#endif


#include "Utility.h"
#include "Direct3D.h"

using namespace Learnings;

namespace
{
	inline std::array<uint16_t, 2> GetWindowSize(HWND hWnd)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);

		return{
			(uint16_t)(rect.right - rect.left),
			(uint16_t)(rect.bottom - rect.top)
		};
	}

	inline DXGI_SAMPLE_DESC QueryMsaaLevel(CComPtr<ID3D11Device> device, uint16_t sampleCount, DXGI_FORMAT dxgiFormat)
	{
		DXGI_SAMPLE_DESC sd{ 1, 0 };

#ifdef _DEBUG
		return sd;
#endif

		UINT msaa = 0;

		HRESULT hr = device->CheckMultisampleQualityLevels(dxgiFormat, sampleCount, &msaa);
		if (SUCCEEDED(hr))
		{
			if (msaa > 0)
			{
				sd.Count = sampleCount;
				sd.Quality = msaa - 1;
			}
		}

		return sd;
	}

	inline DXGI_RATIONAL QueryRefreshRate(CComPtr<IDXGIAdapter> dxgiAdapter, uint16_t width, uint16_t height, DXGI_FORMAT dxgiFormat, BOOL vSync)
	{
		DXGI_RATIONAL refreshRate{ 0, 1 };

		if (vSync)
		{
			HRESULT hr;

			CComPtr<IDXGIOutput> adapterOutput;
			hr = dxgiAdapter->EnumOutputs(0, &adapterOutput);
			ThrowIfFailed(hr, "Failed to enumerate adapter outputs");

			uint32_t displayModesCnt = 0;
			hr = adapterOutput->GetDisplayModeList(dxgiFormat,
												   DXGI_ENUM_MODES_INTERLACED,
												   &displayModesCnt,
												   nullptr);
			ThrowIfFailed(hr, "Failed to get display modes count");


			std::vector<DXGI_MODE_DESC> displayModes(displayModesCnt);
			hr = adapterOutput->GetDisplayModeList(dxgiFormat,
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

	static const DXGI_FORMAT C_SwapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const uint16_t C_MSAAQualityLevel = 4U;
}

GraphicsDevice::GraphicsDevice(HWND hWnd)
	: m_hWnd(hWnd)
{
	CreateDevice();
	CreateSwapChain();
}

GraphicsDevice::~GraphicsDevice()
{
}

void GraphicsDevice::Resize(uint16_t width, uint16_t height)
{
	ResizeSwapChain(width, height);
}

void GraphicsDevice::Present(bool vSync)
{
	m_SwapChain->Present(
		(vSync ? TRUE : FALSE),
		NULL
		);
}

GraphicsDevice::RenderTargetView GraphicsDevice::CreateRenderTargetView(uint16_t index)
{
	HRESULT hr;

	Texture2D buffer = nullptr;
	hr = m_SwapChain->GetBuffer(index, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&buffer.p));
	ThrowIfFailed(hr, "Failed to get Swap Chain buffer");

	RenderTargetView rtv;
	hr = m_Device->CreateRenderTargetView(buffer, 0, &rtv);
	ThrowIfFailed(hr, "Failed to create RenderTargetView from buffer");

	buffer.Release();

	return rtv;
}

GraphicsDevice::DepthStencilView GraphicsDevice::CreateDepthStencilView(uint16_t index)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC desc{};
	hr = m_SwapChain->GetDesc(&desc);
	ThrowIfFailed(hr, "Failed to get swap chain description");

	Texture2D buffer = CreateTexture(desc.BufferDesc.Width,
									 desc.BufferDesc.Height,
									 D3D11_BIND_DEPTH_STENCIL,
									 D3D11_USAGE_DEFAULT,
									 DXGI_FORMAT_D24_UNORM_S8_UINT,
									 desc.SampleDesc,
									 1,
									 1);

	DepthStencilView dsv;
	hr = m_Device->CreateDepthStencilView(buffer, NULL, &dsv);
	ThrowIfFailed(hr, "Failed to create depth stencil view");

	return dsv;
}

D3D11_VIEWPORT GraphicsDevice::GetViewportDesc()
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC desc{};
	hr = m_SwapChain->GetDesc(&desc);
	ThrowIfFailed(hr, "Failed to get swap chain description");

	D3D11_VIEWPORT vd{};
	vd.Width = (float)desc.BufferDesc.Width;
	vd.Height = (float)desc.BufferDesc.Height;
	vd.MinDepth = 0.0f;
	vd.MaxDepth = 1.0f;
	vd.TopLeftX = 0.0f;
	vd.TopLeftY = 0.0f;

	return vd;
}

GraphicsDevice::Texture2D GraphicsDevice::CreateTexture(uint32_t width, uint32_t height, uint32_t bindFlags, D3D11_USAGE usage, DXGI_FORMAT format, DXGI_SAMPLE_DESC sampleDesc, uint16_t arraysize, uint16_t miplevels)
{
	D3D11_TEXTURE2D_DESC td{};

	td.Width = width;
	td.Height = height;

	td.BindFlags = bindFlags;
	td.Usage = usage;
	td.Format = format;
	td.SampleDesc = sampleDesc;

	td.ArraySize = arraysize;
	td.MipLevels = miplevels;

	Texture2D texture;
	HRESULT hr = m_Device->CreateTexture2D(&td, nullptr, &texture);
	ThrowIfFailed(hr, "Failed to create texture");

	return texture;
}

GraphicsDevice::ShaderResourceView GraphicsDevice::CreateShaderResourceView(const Texture2D & texture)
{
	HRESULT hr;

	ShaderResourceView view;
	hr = m_Device->CreateShaderResourceView(texture, nullptr, &view);
	ThrowIfFailed(hr, "Cannot create shader resource view");

	return view;
}

GraphicsDevice::ShaderResourceView GraphicsDevice::CreateShaderResourceView(const std::vector<byte>& ddsData)
{
	ThrowNotImplmented();
	return ShaderResourceView();
}

GraphicsDevice::Buffer GraphicsDevice::CreateBuffer(const void *data, uint32_t size, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, UINT accessFlags)
{
	D3D11_BUFFER_DESC bd{ 0 };
	bd.Usage = usage;
	bd.ByteWidth = size;
	bd.BindFlags = bindFlags;
	bd.CPUAccessFlags = accessFlags;

	D3D11_SUBRESOURCE_DATA bData{ 0 };
	bData.pSysMem = data;

	Buffer buffer;
	HRESULT hr = m_Device->CreateBuffer(&bd, &bData, &(buffer.p));
	ThrowIfFailed(hr, "Failed to create buffer");

	return buffer;
}

GraphicsDevice::VertexShader GraphicsDevice::CreateVertexShader(const std::vector<byte>& vsByteCode)
{
	VertexShader vsh;

	HRESULT hr = m_Device->CreateVertexShader(vsByteCode.data(), vsByteCode.size(), NULL, &vsh);
	ThrowIfFailed(hr, "Failed to create vertex shader");

	return vsh;
}

GraphicsDevice::PixelShader GraphicsDevice::CreatePixelShader(const std::vector<byte>& psByteCode)
{
	PixelShader psh;

	HRESULT hr = m_Device->CreatePixelShader(psByteCode.data(), psByteCode.size(), NULL, &psh);
	ThrowIfFailed(hr, "Failed to create pixel shader");

	return psh;
}

GraphicsDevice::BlendState GraphicsDevice::CreateBlendState(D3D11_BLEND src, D3D11_BLEND srcAlpha, D3D11_BLEND dst, D3D11_BLEND dstAlpha, D3D11_BLEND_OP op, D3D11_BLEND_OP opAlpha)
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

GraphicsDevice::DepthStencilState GraphicsDevice::CreateDepthStencilState(bool depthEnable, bool writeEnable)
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

GraphicsDevice::RasterizerState GraphicsDevice::CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode)
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

GraphicsDevice::SamplerState GraphicsDevice::CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, uint32_t maxAnisotropy)
{
	D3D11_SAMPLER_DESC sd{};

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

GraphicsDevice::Context GraphicsDevice::CreateDeferredContext()
{
	ThrowNotImplmented();
	return Context();
}

GraphicsDevice::Context GraphicsDevice::GetImmediateContext() const
{
	Context ctx;
	m_Device->GetImmediateContext(&ctx);
	return ctx;
}

void GraphicsDevice::CreateDevice()
{
	uint32_t flags = NULL;
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Needed for Direct2d

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
								   nullptr); //&m_Context);
	ThrowIfFailed(hr, "Failed to create direct3d device and context");
}

void GraphicsDevice::CreateSwapChain()
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
	auto size = GetWindowSize(m_hWnd);
	width = size[0];
	height = size[1];

	DXGI_SWAP_CHAIN_DESC sd = { 0 };

	sd.BufferCount = 1;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Format = C_SwapChainFormat;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.RefreshRate = QueryRefreshRate(dxgiAdapter, width, height, C_SwapChainFormat, TRUE);
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc = QueryMsaaLevel(m_Device, C_MSAAQualityLevel, C_SwapChainFormat);
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.Windowed = TRUE;

	hr = dxgiFactory->CreateSwapChain(m_Device, &sd, &m_SwapChain);
	ThrowIfFailed(hr, "Failed to create SwapChain");

	dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
}

void GraphicsDevice::ResizeSwapChain(uint16_t width, uint16_t height)
{
	HRESULT hr;

	hr = m_SwapChain->ResizeBuffers(0, width, height, C_SwapChainFormat, 0);
	ThrowIfFailed(hr, "Failed to resize swap chain buffer");
}
