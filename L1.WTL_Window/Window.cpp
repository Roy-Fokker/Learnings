#include "Window.h"

using namespace Learnings;

//-------------------------------------------------------------------------------------
// ATL Window stuff
LRESULT Window::OnClose(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	// TODO: Do Callback, ask Application if it's okay to destroy window
	bHandled = DestroyWindow();

	return 0;
}

LRESULT Window::OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostQuitMessage(0);

	bHandled = TRUE;
	return 0;
}

LRESULT Window::OnPaint(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PAINTSTRUCT ps{ 0 };
	HDC hdc = BeginPaint(&ps);
	EndPaint(&ps);

	bHandled = TRUE;
	return 0;
}

LRESULT Window::OnActivate(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (wParam)
	{
		m_IsActive = true;
	}
	else
	{
		m_IsActive = false;
	}

	// TODO: Do Callback, let application know

	bHandled = TRUE;
	return 0;
}

LRESULT Window::OnResize(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (!m_IsResizing)
	{
		uint16_t w = (uint16_t)LOWORD(lParam);
		uint16_t h = (uint16_t)HIWORD(lParam);

		switch (wParam)
		{
			case SIZE_MINIMIZED:
				break;
			case SIZE_MAXIMIZED:
				break;
			case SIZE_RESTORED:
				break;
		}

		// TODO: Do Callback, let application know
	}

	bHandled = TRUE;
	return 0;
}

LRESULT Window::OnSizing(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_hWnd)
	{
		switch (msg)
		{
			case WM_ENTERSIZEMOVE:
				m_IsResizing = true;
				break;
			case WM_EXITSIZEMOVE:
				m_IsResizing = false;
				break;
		}

		// TODO: Do Callback, let application know
	}

	bHandled = TRUE;
	return 0;
}


//-------------------------------------------------------------------------------------
// Our Window stuff

Window::Window(uint16_t width, uint16_t height, const std::wstring &title, Style style)
{
	Create(nullptr,
		   RECT{ 0, 0, width, height },
		   title.c_str(),
		   WS_OVERLAPPEDWINDOW,
		   WS_EX_OVERLAPPEDWINDOW | WS_EX_LAYERED | WS_EX_COMPOSITED);

	Resize(width, height);
	Restyle(style);
}

Window::~Window()
{
}

void Window::Restyle(Style style)
{
	DWORD adStyle = 0, adExStyle = 0;
	DWORD rmStyle = 0, rmExStyle = 0;
	switch (style)
	{
		case Window::Windowed:
			m_IsFullscreen = false;
			adStyle = WS_OVERLAPPEDWINDOW;
			adExStyle = WS_EX_OVERLAPPEDWINDOW;
			rmStyle = WS_POPUP | WS_MINIMIZEBOX;
			break;
		case Window::Borderless:
			m_IsFullscreen = false;
			adStyle = WS_POPUP | WS_MINIMIZEBOX;
			rmStyle = WS_OVERLAPPEDWINDOW;
			rmExStyle = WS_EX_OVERLAPPEDWINDOW;
			break;
		case Window::Fullscreen:
			m_IsFullscreen = true;
			adStyle = WS_POPUP | WS_MINIMIZEBOX;
			rmStyle = WS_OVERLAPPEDWINDOW;
			rmExStyle = WS_EX_OVERLAPPEDWINDOW;
			break;
	}

	ModifyStyle(rmStyle, adStyle);
	ModifyStyleEx(rmExStyle, adExStyle);
	CenterWindow();

	if (style == Window::Fullscreen)
	{
		MONITORINFO mi{ 0 };
		GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &mi);

		SetWindowPos(HWND_TOP,
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

void Window::Retitle(const std::wstring &title)
{
	SetWindowText(title.c_str());
}

void Window::Show(bool show)
{
	if (!m_hWnd)
		return;

	if (show)
	{
		ShowWindow(SW_SHOW);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

bool Window::ProcessMessages()
{
	MSG msg{ 0 };
	BOOL bRet = TRUE;

	do
	{
		if (m_IsActive)
			bRet = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		else
			bRet = GetMessage(&msg, nullptr, 0, 0);

		if (bRet)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			return false;
		}

	} while (bRet);

	return true;
}
