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
		typedef CComPtr<ID3D11Texture2D> DepthStencilBuffer;

		typedef CComPtr<ID3D11Buffer> Buffer;
		typedef CComPtr<ID3D11VertexShader> VertexShader;
		typedef CComPtr<ID3D11PixelShader> PixelShader;
		typedef CComPtr<ID3D11InputLayout> InputLayout;

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

		Buffer CreateBuffer(uint32_t size, const void *data, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, UINT accessFlags);
		VertexShader CreateVertexShader(uint32_t size, const void *vs);
		PixelShader CreatePixelShader(uint32_t size, const void *ps);
		InputLayout CreateInputLayout(uint32_t elemCount, const D3D11_INPUT_ELEMENT_DESC *elemDesc, uint32_t vsSize, const void *vs);
		bool CheckInputLayout(uint32_t elemCount, const D3D11_INPUT_ELEMENT_DESC *elemDesc, uint32_t vsSize, const void *vs);

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
		DepthStencilBuffer m_DepthStencilBuffer;
		DepthStencilView m_DepthStencilView;

		BOOL m_vSync = TRUE;

		HWND m_hWnd;
	};
}