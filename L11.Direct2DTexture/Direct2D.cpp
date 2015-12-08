#include <stdexcept>


#include "Direct2D.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define ThrowIfFailed(hr, msg) \
if ( FAILED(hr) ) \
{ \
	throw std::runtime_error(msg); \
}

using namespace Learnings;

Direct2d::Direct2d(DxgiDevice device)
{
	CreateDevice(device);
}

Direct2d::~Direct2d()
{
	DeleteDevice();
}

Direct2d::Bitmap Direct2d::CreateBitmap(Surface surface)
{
	HRESULT hr;

	DXGI_SURFACE_DESC sd{};
	hr = surface->GetDesc(&sd);

	D2D1_BITMAP_PROPERTIES1 props{};
	props.pixelFormat.format = sd.Format;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
	Bitmap bitmap;;
	hr = m_Context->CreateBitmapFromDxgiSurface(surface, props, &bitmap);
	ThrowIfFailed(hr, "Failed to create bitmap from dxgi surface");

	return bitmap;
}

Direct2d::SolidColorBrush Direct2d::CreateSolidBrush(D2D1::ColorF color)
{
	HRESULT hr;

	SolidColorBrush brush;
	hr = m_Context->CreateSolidColorBrush(color, &brush);
	ThrowIfFailed(hr, "Failed to create a solid brush");

	return brush;
}

Direct2d::Context Direct2d::GetContext() const
{
	return m_Context;
}

void Direct2d::CreateDevice(DxgiDevice device)
{
	HRESULT hr;
	hr = D2D1CreateDevice(device, nullptr, &m_Device);
	ThrowIfFailed(hr, "Failed to create Direct2D device");

	D2D1_DEVICE_CONTEXT_OPTIONS opts = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	hr = m_Device->CreateDeviceContext(opts, &m_Context);
	ThrowIfFailed(hr, "Failed to create Direct2D device context");
}

void Direct2d::DeleteDevice()
{
	m_Context.Release();
	m_Device.Release();
}
