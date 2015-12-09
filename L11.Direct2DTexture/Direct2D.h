#pragma once

#include <cstdint>
#include <d2d1_1.h>
#include <dwrite.h>
#include <atlbase.h>

namespace Learnings
{
	class Direct2d
	{
	public:
		typedef CComPtr<ID2D1Device> Device;
		typedef CComPtr<ID2D1DeviceContext> Context;

		typedef CComPtr<IDXGIDevice> DxgiDevice;
		typedef CComPtr<IDXGISurface1> Surface;
		typedef CComPtr<ID2D1Bitmap1> Bitmap;
		typedef CComPtr<ID2D1SolidColorBrush> SolidColorBrush;

		typedef CComPtr<IDWriteFactory> WriteFactory;
		typedef CComPtr<IDWriteTextLayout> TextLayout;
		typedef CComPtr<IDWriteTextFormat> TextFormat;

	public:
		Direct2d(DxgiDevice device);
		~Direct2d();

		Bitmap CreateBitmap(Surface surface);
		SolidColorBrush CreateSolidBrush(D2D1::ColorF color);

		TextFormat CreateTextFormat(const std::wstring &font, float size);
		TextLayout CreateTextLayout(const std::wstring &text, float width, float height, TextFormat format);

		Context GetContext() const;

	private:
		void CreateDevice(DxgiDevice device);
		void DeleteDevice();

	private:
		Device m_Device;
		Context m_Context;
		WriteFactory m_WriteFactory;
	};
}