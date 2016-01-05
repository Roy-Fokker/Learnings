

#include "Window.h"

using namespace Learnings;

Window::Window(const Desc &desc, const Callback &callback)
	: m_Callback(callback), 
	m_IsActive(false), 
	m_IsFullscreen(false),
	m_IsResizing(false),
	m_CurrentStyle(Style::Windowed)
{
	auto hInstance = GetModuleHandle(NULL);
	
	Create(nullptr,
		   RECT{ 0, 0, desc.width, desc.height },
		   desc.title.c_str(),
		   WS_POPUP | WS_BORDER,
		   WS_EX_COMPOSITED
		   );

	if (desc.iconResId)
	{
		auto icon = LoadIcon(hInstance, MAKEINTRESOURCE(desc.iconResId));
		SetIcon(icon);
	}

	SetStyle(desc.style);
	SetSize(desc.width, desc.height);
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

void Window::SetSize(uint16_t width, uint16_t height)
{
	if (!m_IsFullscreen)
	{
		ResizeClient(width, height);
		CenterWindow();
	}
}

POINT Window::GetSize() const
{
	RECT rect{};
	GetClientRect(&rect);

	return POINT{
		(rect.left - rect.right),
		(rect.bottom - rect.top)
	};
}

void Window::SetStyle(Style style)
{
	m_IsResizing = true; // changing styles will fire WM_SIZE
	m_CurrentStyle = style;

	DWORD adStyle = 0, adExStyle = 0;
	DWORD rmStyle = WS_POPUP | WS_BORDER | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
		rmExStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_LAYERED | WS_EX_COMPOSITED;
	switch (style)
	{
	case Style::Windowed:
		m_IsFullscreen = false;
		adStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME; // Prevent resizing Window with ^ WS_Thickframe
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

	m_IsResizing = false;
}

Window::Style Window::GetStyle() const
{
	return m_CurrentStyle;
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

LRESULT Window::OnPaint(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PAINTSTRUCT ps{ 0 };
	HDC hdc = BeginPaint(&ps);
	EndPaint(&ps);

	bHandled = TRUE;
	return 0;
}

LRESULT Window::OnSystemCommand(UINT msg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (wParam == SC_KEYMENU && lParam == VK_RETURN)
	{
		if (m_IsFullscreen)
		{
			ShowWindow(SW_RESTORE);
		}
		else
		{
			ShowWindow(SW_MAXIMIZE);
		}
	}

	return DefWindowProc();
}

LRESULT Window::OnSizing(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_hWnd)
	{
		switch (msg)
		{
		case WM_ENTERSIZEMOVE:
			OutputDebugStringA("Enter Size Move\n");
			m_IsResizing = true;
			m_Callback(Message::Pause, 0, 0);
			break;
		case WM_EXITSIZEMOVE:
			OutputDebugStringA("Exit Size Move\n");
			m_IsResizing = false;
			m_Callback(Message::Resume, 0, 0);
			break;
		}
	}

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
			SetStyle(Style::Fullscreen);
			m_Callback(Message::Resized, w, h);
			break;
		case SIZE_RESTORED:
			SetStyle(Style::Windowed);
			m_Callback(Message::Resized, w, h);
			break;
		default:
			//m_Callback(Message::Resized, w, h);
			break;
		}
	}

	bHandled = TRUE;
	return 0;
}

LRESULT Window::OnActivate(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (wParam)
	{
		m_IsActive = true;
		m_Callback(Message::Resume, 0, 0);
	}
	else
	{
		m_IsActive = false;
		m_Callback(Message::Pause, 0, 0);
	}

	// TODO: Do Callback, let application know

	bHandled = TRUE;
	return 0;
}

#pragma endregion
