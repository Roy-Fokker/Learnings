#include "Window.h"

#include "resource.h"

using namespace Learnings;

Window::Window(const Desc &desc, const Callback &callback)
	: m_Callback(callback), m_IsActive(false), m_IsFullscreen(false)
{
	auto hInstance = GetModuleHandle(NULL);
	auto icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	GetWndClassInfo().m_wc.hIcon = icon;

	Create(nullptr,
		   RECT{ 0, 0, desc.width, desc.height },
		   desc.title.c_str(),
		   WS_POPUP | WS_SIZEBOX,
		   WS_EX_COMPOSITED
		   );

	Restyle(desc.style);
	Resize(desc.width, desc.height);
}

Window::~Window()
{
}

void Window::Show()
{
	if (m_hWnd)
	{
		ShowWindow(SW_SHOW);
	}
}

void Window::Restyle(Style style)
{
	DWORD adStyle = 0, adExStyle = 0;
	DWORD rmStyle = WS_POPUP | WS_SIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
		rmExStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_LAYERED | WS_EX_COMPOSITED;
	switch (style)
	{
		case Style::Windowed:
			m_IsFullscreen = false;
			adStyle = WS_OVERLAPPEDWINDOW;
			adExStyle = WS_EX_OVERLAPPEDWINDOW;
			break;
		case Style::Borderless:
			m_IsFullscreen = true;
			adStyle = WS_POPUP | WS_MINIMIZEBOX;
			break;
		case Style::Fullscreen:
			m_IsFullscreen = true;
			adStyle = WS_POPUP | WS_MINIMIZEBOX;
			break;
	}

	ModifyStyle(rmStyle, adStyle, SWP_FRAMECHANGED);
	ModifyStyleEx(rmExStyle, adExStyle, SWP_FRAMECHANGED);
	CenterWindow();

	if (m_IsFullscreen)
	{
		MONITORINFO mi{ sizeof(MONITORINFO) };
		GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &mi);

		BOOL bRet = SetWindowPos(HWND_TOP,
								 &mi.rcMonitor,
								 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

void Window::Resize(uint16_t width, uint16_t height)
{
	if (!m_IsFullscreen)
	{
		ResizeClient(width, height);
		CenterWindow();
	}
}

void Window::Update()
{
	MSG msg{};
	BOOL msgRet = TRUE;

	do
	{
		if (m_IsActive)
		{
			msgRet = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		}
		else
		{
			msgRet = GetMessage(&msg, nullptr, 0, 0);
		}

		if (msgRet)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
	
	} while (msgRet);
}


#pragma region ATL Window Specific

LRESULT Window::OnClose(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_Callback(Message::Close, 0, 0))
	{
		bHandled = DestroyWindow();
	}

	return 0;
}

LRESULT Window::OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostQuitMessage(0);

	bHandled = TRUE;
	return 0;
}

#pragma endregion
