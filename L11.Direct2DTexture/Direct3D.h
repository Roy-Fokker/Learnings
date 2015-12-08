#pragma once

#include <cstdint>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <atlbase.h>

namespace Learnings
{
	class Direct3d
	{
	public:
		typedef CComPtr<ID3D11DeviceContext> Context;

		typedef CComPtr<ID3D11RenderTargetView> RenderTargetView;
		typedef CComPtr<ID3D11DepthStencilView> DepthStencilView;
		typedef CComPtr<ID3D11Texture2D> Texture2d;

		typedef CComPtr<ID3D11Buffer> Buffer;
		typedef CComPtr<ID3D11VertexShader> VertexShader;
		typedef CComPtr<ID3D11PixelShader> PixelShader;
		typedef CComPtr<ID3D11InputLayout> InputLayout;

		typedef CComPtr<ID3D11ShaderResourceView> ShaderResourceView;

		typedef CComPtr<ID3D11BlendState> BlendState;
		typedef CComPtr<ID3D11DepthStencilState> DepthStencilState;
		typedef CComPtr<ID3D11RasterizerState> RasterizerState;
		typedef CComPtr<ID3D11SamplerState> SamplerState;

	private:
		typedef CComPtr<ID3D11Device> Device;
		typedef CComPtr<IDXGISwapChain> SwapChain;
		
	public:
		Direct3d(HWND hWnd);
		~Direct3d();

		void Clear();
		void Present();

		void Resize();
	
		Context GetContext() const;
		Device GetDevice() const;

		Buffer CreateBuffer(uint32_t size, const void *data, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, UINT accessFlags);
		VertexShader CreateVertexShader(uint32_t size, const void *vs);
		PixelShader CreatePixelShader(uint32_t size, const void *ps);
		InputLayout CreateInputLayout(uint32_t elemCount, const D3D11_INPUT_ELEMENT_DESC *elemDesc, uint32_t vsSize, const void *vs);
		bool CheckInputLayout(uint32_t elemCount, const D3D11_INPUT_ELEMENT_DESC *elemDesc, uint32_t vsSize, const void *vs);
		ShaderResourceView CreateShaderResourceView(uint32_t size, const uint8_t *tex);
		ShaderResourceView CreateShaderResourceView(Texture2d texture);
		BlendState CreateBlendState(D3D11_BLEND src, D3D11_BLEND srcAlpha, D3D11_BLEND dst, D3D11_BLEND dstAlpha, D3D11_BLEND_OP op, D3D11_BLEND_OP opAlpha);
		DepthStencilState CreateDepthStencilState(bool depthEnable, bool writeEnable);
		RasterizerState CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode);
		SamplerState CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, uint32_t maxAnisotropy);
		Texture2d CreateTexture2d(uint32_t width, uint32_t height, uint32_t bindFlags, D3D11_USAGE usage, DXGI_FORMAT format, DXGI_SAMPLE_DESC sampleDesc, uint16_t arraysize, uint16_t miplevels);

	private:
		void CreateDevice();
		void DeleteDevice();

		void CreateSwapChain();
		void DeleteSwapChain();

		void CreateViewBuffer();
		void DeleteViewBuffer();

		void CreateRenderTargetView();
		void DeleteRenderTargetView();

		void CreateDepthStencilBuffer(uint16_t width, uint16_t height);
		void DeleteDepthStencilBuffer();

		void CreateDepthStencilView();
		void DeleteDepthStencilView();

	private:
		Device m_Device;
		SwapChain m_SwapChain;
		Context m_Context;

		RenderTargetView m_RenderTargetView;
		Texture2d m_DepthStencilBuffer;
		DepthStencilView m_DepthStencilView;

		BOOL m_vSync = FALSE;

		HWND m_hWnd;
	};
};