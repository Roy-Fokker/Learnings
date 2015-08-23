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
		typedef CComPtr<ID3D11Device> Device;
		typedef CComPtr<IDXGISwapChain> SwapChain;
		typedef CComPtr<ID3D11DeviceContext> Context;

		typedef CComPtr<ID3D11RenderTargetView> RenderTargetView;
		typedef CComPtr<ID3D11DepthStencilView> DepthStencilView;
		typedef CComPtr<ID3D11Texture2D> DepthStencilBuffer;
		
	public:
		Direct3d(HWND hWnd);
		~Direct3d();

		void Draw();
		void Resize();

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