#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#if defined(_DEBUG) || defined(DEBUG)
	#pragma comment(lib, "dxguid.lib")
#endif


#include "Direct3D.h"

using namespace Learnings;

Direct3D::Direct3D(HWND hWnd)
	: m_hWnd(hWnd)
{
}

Direct3D::~Direct3D()
{
}

void Direct3D::Resize(uint16_t width, uint16_t height)
{
	OutputDebugStringA("Resize Direct3D\n");
}
