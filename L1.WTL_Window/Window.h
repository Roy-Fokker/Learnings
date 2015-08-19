#pragma once

#include <string>
#include <cstdint>
#include <functional>

#include <Windows.h>
#include <atlbase.h>
#include <atlwin.h>

namespace Learnings
{
	class Window : public CWindowImpl<Window>
	{
	//-------------------------------------------------------------------------------------
	// ATL Window stuff
	public:
		BEGIN_MSG_MAP(Window)
			MESSAGE_HANDLER(WM_CLOSE, OnClose)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_ACTIVATEAPP, OnActivate)
			MESSAGE_HANDLER(WM_SIZE, OnResize)
			MESSAGE_RANGE_HANDLER(WM_ENTERSIZEMOVE, WM_EXITSIZEMOVE, OnSizing)
		END_MSG_MAP()

	private:
		LRESULT OnClose(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnPaint(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnActivate(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnResize(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnSizing(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//-------------------------------------------------------------------------------------
	// Our Window stuff
	public:
		enum Style
		{
			Windowed,
			Borderless,
			Fullscreen
		};

		enum Message
		{
			Activate,
			Deactivate,
			Pause,
			Resume,
			Resized,
			Maximized,
			Close
		};

		typedef std::function<bool(Message, uint16_t, uint16_t)> Callback;
		
	public:
		Window(uint16_t width, uint16_t height, const std::wstring &title, Style style, const Callback &callback);
		~Window();

		void Restyle(Style style);
		void Resize(uint16_t width, uint16_t height);
		void Retitle(const std::wstring &title);

		void Show(int showCmd);

		bool ProcessMessages();

	private:
		bool m_IsActive = false;
		bool m_IsResizing = false;
		bool m_IsFullscreen = false;


		Callback m_Callback;
	};

}