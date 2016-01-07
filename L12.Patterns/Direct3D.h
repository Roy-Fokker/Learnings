#pragma once

#include <cstdint>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <atlbase.h>

#include <vector>
#include <array>

#include "Services.h"

namespace Learnings
{
	class GraphicsDevice : public Service
	{
	public:
		typedef CComPtr<ID3D11DeviceContext> Context;

		typedef CComPtr<ID3D11RenderTargetView> RenderTargetView;
		typedef CComPtr<ID3D11DepthStencilView> DepthStencilView;
		typedef CComPtr<ID3D11Texture2D> Texture2D;

		typedef CComPtr<ID3D11Buffer> Buffer;
		typedef CComPtr<ID3D11VertexShader> VertexShader;
		typedef CComPtr<ID3D11PixelShader> PixelShader;
		typedef CComPtr<ID3D11InputLayout> InputLayout;

		typedef CComPtr<ID3D11BlendState> BlendState;
		typedef CComPtr<ID3D11DepthStencilState> DepthStencilState;
		typedef CComPtr<ID3D11RasterizerState> RasterizerState;
		typedef CComPtr<ID3D11SamplerState> SamplerState;

		typedef CComPtr<ID3D11ShaderResourceView> ShaderResourceView;

		typedef CComPtr<ID3D11CommandList> CommandList;

	private:
		typedef CComPtr<ID3D11Device> Device;
		typedef CComPtr<IDXGISwapChain> SwapChain;

	public:
		GraphicsDevice(HWND hWnd);
		~GraphicsDevice();

		void Resize(uint16_t width, uint16_t height);

		void Present(bool vSync);

		RenderTargetView CreateRenderTargetView(uint16_t index);
		DepthStencilView CreateDepthStencilView(uint16_t index);
		D3D11_VIEWPORT GetViewportDesc();

		Texture2D CreateTexture(uint32_t width, uint32_t height, uint32_t bindFlags, D3D11_USAGE usage, DXGI_FORMAT format, DXGI_SAMPLE_DESC sampleDesc, uint16_t arraysize, uint16_t miplevels);
		ShaderResourceView CreateShaderResourceView(const Texture2D &texture);
		ShaderResourceView CreateShaderResourceView(const std::vector<byte> &ddsData);

		Buffer CreateBuffer(const void *data, uint32_t size, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, UINT accessFlags);

		template <uint16_t SIZE>
		InputLayout CreateInputLayout(const std::array<D3D11_INPUT_ELEMENT_DESC, SIZE> &elements, const std::vector<byte> &vsByteCode);
		template <uint16_t SIZE>
		bool CheckInputLayout(const std::array<D3D11_INPUT_ELEMENT_DESC, SIZE> &elements, const std::vector<byte> &vsByteCode);

		VertexShader CreateVertexShader(const std::vector<byte> &vsByteCode);
		PixelShader CreatePixelShader(const std::vector<byte> &psByteCode);

		BlendState CreateBlendState(D3D11_BLEND src, D3D11_BLEND srcAlpha, D3D11_BLEND dst, D3D11_BLEND dstAlpha, D3D11_BLEND_OP op, D3D11_BLEND_OP opAlpha);
		DepthStencilState CreateDepthStencilState(bool depthEnable, bool writeEnable);
		RasterizerState CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode);
		SamplerState CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, uint32_t maxAnisotropy);

		Context CreateDeferredContext();
		Context GetImmediateContext() const;

	private:
		void CreateDevice();
		void CreateSwapChain();
		void ResizeSwapChain(uint16_t width, uint16_t height);

	private:
		HWND m_hWnd;

		Device m_Device;
		SwapChain m_SwapChain;
	};

	class RenderTarget : public Service
	{
	public:
		RenderTarget(GraphicsDevice::Context context, GraphicsDevice::RenderTargetView rtv, GraphicsDevice::DepthStencilView dsv, D3D11_VIEWPORT viewport);
		~RenderTarget();
		
		void Clear(const std::array<float, 4u> &color);

		void Finish();

		GraphicsDevice::CommandList CommandList() const;
		
	private:
		GraphicsDevice::Context m_Context;
		GraphicsDevice::RenderTargetView m_RTV;
		GraphicsDevice::DepthStencilView m_DSV;
		D3D11_VIEWPORT m_Viewport;

		GraphicsDevice::CommandList m_CommandList;
	};
}

#include "Direct3D.inl"